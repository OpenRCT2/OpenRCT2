/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../paint/Paint.h"
#include "../paint/support/MetalSupports.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../paint/track/Support.h"
#include "../world/Map.h"

class StationObject;
struct Ride;

constexpr uint8_t kTrackMap2x2[][4] = {
    { 0, 1, 2, 3 },
    { 1, 3, 0, 2 },
    { 3, 2, 1, 0 },
    { 2, 0, 3, 1 },
};
constexpr uint8_t kEdges2x2[] = {
    EDGE_NE | EDGE_NW,
    EDGE_NE | EDGE_SE,
    EDGE_SW | EDGE_NW,
    EDGE_SW | EDGE_SE,
};

constexpr uint8_t kTrackMap3x3[][9] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 0, 3, 5, 7, 2, 8, 1, 6, 4 },
    { 0, 7, 8, 6, 5, 4, 3, 1, 2 },
    { 0, 6, 4, 1, 8, 2, 7, 3, 5 },
};
// clang-format off
constexpr uint8_t kEdges3x3[] = {
    0,
    EDGE_NE | EDGE_NW,
    EDGE_NE,
    EDGE_NE | EDGE_SE,
    EDGE_NW,
    EDGE_SE,
    EDGE_SW | EDGE_NW,
    EDGE_SW | EDGE_SE,
    EDGE_SW,
};
// clang-format on

constexpr uint8_t kTrackMap4x4[][16] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    { 3, 7, 11, 15, 2, 6, 10, 14, 1, 5, 9, 13, 0, 4, 8, 12 },
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
    { 12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3 },
};
// clang-format off
constexpr uint8_t kEdges4x4[] = {
    EDGE_NE | EDGE_NW,
    EDGE_NE,
    EDGE_NE,
    EDGE_NE | EDGE_SE,
    EDGE_NW,
    0,
    0,
    EDGE_SE,
    EDGE_NW,
    0,
    0,
    EDGE_SE,
    EDGE_NW | EDGE_SW,
    EDGE_SW,
    EDGE_SW,
    EDGE_SW | EDGE_SE,
};
// clang-format on

constexpr uint8_t kTrackMap1x4[][4] = {
    { 0, 1, 2, 3 },
    { 2, 3, 0, 1 },
    { 2, 3, 0, 1 },
    { 0, 1, 2, 3 },
};

constexpr MetalSupportPlace kDiagSupportPlacement[] = {
    MetalSupportPlace::LeftCorner,
    MetalSupportPlace::TopCorner,
    MetalSupportPlace::RightCorner,
    MetalSupportPlace::BottomCorner,
};

enum
{
    SPR_FLOOR_PLANKS = 3395,
    SPR_FLOOR_PLANKS_90_DEG = 3396,
    SPR_FLOOR_PLANKS_SLOPE_NE_SW = 3397,
    SPR_FLOOR_PLANKS_SLOPE_SE_NW = 3398,
    SPR_FLOOR_PLANKS_SLOPE_SW_NE = 3399,
    SPR_FLOOR_PLANKS_SLOPE_NW_SE = 3400,
    SPR_FLOOR_PLANKS_N_SEGMENT = 3401,
    SPR_FLOOR_PLANKS_E_SEGMENT = 3402,
    SPR_FLOOR_PLANKS_S_SEGMENT = 3403,
    SPR_FLOOR_PLANKS_W_SEGMENT = 3404,
    SPR_FLOOR_METAL = 14567,
    SPR_FENCE_METAL_NE = 14568,
    SPR_FENCE_METAL_SE = 14569,
    SPR_FENCE_METAL_SW = 14570,
    SPR_FENCE_METAL_NW = 14571,

    SPR_FLOOR_METAL_B = 14989,
    SPR_FENCE_METAL_B_NE = 14990,
    SPR_FENCE_METAL_B_SE = 14991,
    SPR_FENCE_METAL_B_SW = 14992,
    SPR_FENCE_METAL_B_NW = 14993,

    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW = 16870,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW = 16871,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 16872,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 16873,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 16874,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 16875,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 16876,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 16877,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 16878,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 16879,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 16880,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 16881,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 16882,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 16883,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_SW_NE = 16884,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_NW_SE = 16885,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_NE_SW = 16886,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_SE_NW = 16887,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_SW_NE = 16888,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_NW_SE = 16889,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_NE_SW = 16890,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_SE_NW = 16891,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_SW_NE = 16892,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_NW_SE = 16893,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_NE_SW = 16894,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_SE_NW = 16895,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_SW_NW = 16896,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_NW_NE = 16897,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_NE_SE = 16898,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_SE_SW = 16899,

    SPR_FLOOR_CORK_SE_SW = 22134,
    SPR_FLOOR_CORK_SW = 22135,
    SPR_FLOOR_CORK_SE = 22136,
    SPR_FLOOR_CORK = 22137,
    SPR_FENCE_ROPE_NE = 22138,
    SPR_FENCE_ROPE_SE = 22139,
    SPR_FENCE_ROPE_SW = 22140,
    SPR_FENCE_ROPE_NW = 22141,

    SPR_STATION_PLATFORM_SW_NE = 22362,
    SPR_STATION_PLATFORM_NW_SE = 22363,
    SPR_STATION_PLATFORM_FENCED_SW_NE = 22364,
    SPR_STATION_PLATFORM_FENCED_NW_SE = 22365,
    SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE = 22366,
    SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE = 22367,
    SPR_STATION_PLATFORM_BEGIN_SW_NE = 22368,
    SPR_STATION_PLATFORM_BEGIN_NW_SE = 22369,
    SPR_STATION_FENCE_SW_NE = 22370,
    SPR_STATION_FENCE_NW_SE = 22371,
    SPR_STATION_BEGIN_ANGLE_FENCE_SW_NE = 22372,
    SPR_STATION_BEGIN_ANGLE_FENCE_NW_SE = 22373,
    SPR_STATION_FENCE_SMALL_NW_SE = 22374,
    SPR_STATION_FENCE_SMALL_SW_NE = 22375,

    SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_SW_NE = 22380,
    SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_NW_SE = 22381,
    SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_SW_NE = 22382,
    SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_NW_SE = 22383,
    SPR_STATION_LIGHT_BACK_NE_SW = 22384,
    SPR_STATION_LIGHT_BACK_NW_SE = 22385,
    SPR_STATION_LIGHT_BACK_ANGLE_FENCED_NE_SW = 22386,
    SPR_STATION_LIGHT_BACK_ANGLE_FENCED_NW_SE = 22387,
    SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE = 22388,
    SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE = 22389,
    SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE = 22390,
    SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE = 22391,
    SPR_STATION_INVERTED_FENCE_SW_NE = 22392,
    SPR_STATION_INVERTED_FENCE_NW_SE = 22393,
    SPR_STATION_INVERTED_BEGIN_ANGLE_FENCE_SW_NE = 22394,
    SPR_STATION_INVERTED_BEGIN_ANGLE_FENCE_NW_SE = 22395,
    SPR_STATION_INVERTED_LIGHT_BACK_ANGLE_FENCED_NE_SW = 22396,
    SPR_STATION_INVERTED_LIGHT_BACK_ANGLE_FENCED_NW_SE = 22397,
    SPR_STATION_INVERTED_BAR_C_SW_NE = 22398,
    SPR_STATION_INVERTED_BAR_C_NW_SE = 22399,
    SPR_STATION_INVERTED_BAR_D_SW_NE = 22400,
    SPR_STATION_INVERTED_BAR_D_NW_SE = 22401,
    SPR_STATION_INVERTED_BAR_E_SW_NE = 22402,
    SPR_STATION_INVERTED_BAR_E_NW_SE = 22403,
    SPR_STATION_PIER_EDGE_SE = 22404,
    SPR_STATION_PIER_EDGE_SW = 22405,
    SPR_STATION_PIER_EDGE_NW = 22406,
    SPR_STATION_PIER_EDGE_NE = 22407,
    SPR_STATION_PIER_EDGE_NW_FENCED = 22408,
    SPR_STATION_PIER_EDGE_NE_FENCED = 22409,
    SPR_STATION_PIER_FENCE_SE = 22410,
    SPR_STATION_PIER_FENCE_SW = 22411,
    SPR_STATION_NARROW_EDGE_SE = 22412,
    SPR_STATION_NARROW_EDGE_SW = 22413,
    SPR_STATION_NARROW_EDGE_FENCED_NW = 22414,
    SPR_STATION_NARROW_EDGE_FENCED_NE = 22415,
    SPR_STATION_NARROW_EDGE_NW = 22416,
    SPR_STATION_NARROW_EDGE_NE = 22417,
    SPR_STATION_INVERTED_BAR_F_SW_NE = 22418,
    SPR_STATION_INVERTED_BAR_F_NW_SE = 22419,
    SPR_STATION_INVERTED_BAR_0_SW_NE = 22420,
    SPR_STATION_INVERTED_BAR_0_NW_SE = 22421,
    SPR_STATION_INVERTED_BAR_A_SW_NE = 22422,
    SPR_STATION_INVERTED_BAR_A_NW_SE = 22423,
    SPR_STATION_INVERTED_BAR_B_SW_NE = 22424,
    SPR_STATION_INVERTED_BAR_B_NW_SE = 22425,
    SPR_STATION_BASE_A_SW_NE = 22426,
    SPR_STATION_BASE_A_NW_SE = 22427,
    SPR_STATION_BASE_B_SW_NE = 22428,
    SPR_STATION_BASE_B_NW_SE = 22429,
    SPR_STATION_BASE_C_SW_NE = 22430,
    SPR_STATION_BASE_C_NW_SE = 22431,
    SPR_STATION_BASE_D = 22432, // Metal plate without borders, used for some stations and for the on-ride photo segment.

    SPR_ON_RIDE_PHOTO_CAMERA_SMALL_N = 23485,
    SPR_ON_RIDE_PHOTO_CAMERA_SMALL_E = 23486,
    SPR_ON_RIDE_PHOTO_CAMERA_SMALL_S = 23487,
    SPR_ON_RIDE_PHOTO_CAMERA_SMALL_W = 23488,
    SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_N = 23489,
    SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_E = 23490,
    SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_S = 23491,
    SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_W = 23492,
    SPR_ON_RIDE_PHOTO_SIGN_SMALL_SW_NE = 23493,
    SPR_ON_RIDE_PHOTO_SIGN_SMALL_NW_SE = 23494,
    SPR_ON_RIDE_PHOTO_SIGN_SMALL_NE_SW = 23495,
    SPR_ON_RIDE_PHOTO_SIGN_SMALL_SE_NW = 23496,

    SPR_ON_RIDE_PHOTO_CAMERA_N = 25615,
    SPR_ON_RIDE_PHOTO_CAMERA_E = 25616,
    SPR_ON_RIDE_PHOTO_CAMERA_S = 25617,
    SPR_ON_RIDE_PHOTO_CAMERA_W = 25618,
    SPR_ON_RIDE_PHOTO_CAMERA_FLASH_N = 25619,
    SPR_ON_RIDE_PHOTO_CAMERA_FLASH_E = 25620,
    SPR_ON_RIDE_PHOTO_CAMERA_FLASH_S = 25621,
    SPR_ON_RIDE_PHOTO_CAMERA_FLASH_W = 25622,
    SPR_ON_RIDE_PHOTO_SIGN_SW_NE = 25623,
    SPR_ON_RIDE_PHOTO_SIGN_NW_SE = 25624,
    SPR_ON_RIDE_PHOTO_SIGN_NE_SW = 25625,
    SPR_ON_RIDE_PHOTO_SIGN_SE_NW = 25626,

    SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_SW_NE_FRAME_0 = 28865,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_SW_NE_FRAME_1 = 28866,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_SW_NE_FRAME_2 = 28867,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_SW_NE_FRAME_3 = 28868,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_0 = 28869,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_1 = 28870,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_2 = 28871,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_3 = 28872,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_0 = 28873,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_1 = 28874,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_2 = 28875,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_3 = 28876,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_0 = 28877,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_1 = 28878,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_2 = 28879,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_3 = 28880,
};

enum
{
    STATION_VARIANT_BASIC,
    STATION_VARIANT_1,
    STATION_VARIANT_TALL,
};

enum
{
    MAZE_ENTRY_FLAG_0 = (1 << 0),
    MAZE_ENTRY_FLAG_1 = (1 << 1),
    MAZE_ENTRY_FLAG_2 = (1 << 2),
    MAZE_ENTRY_FLAG_3 = (1 << 3),
    MAZE_ENTRY_FLAG_4 = (1 << 4),
    MAZE_ENTRY_FLAG_5 = (1 << 5),
    MAZE_ENTRY_FLAG_6 = (1 << 6),
    MAZE_ENTRY_FLAG_7 = (1 << 7),
    MAZE_ENTRY_FLAG_8 = (1 << 8),
    MAZE_ENTRY_FLAG_9 = (1 << 9),
    MAZE_ENTRY_FLAG_10 = (1 << 10),
    MAZE_ENTRY_FLAG_11 = (1 << 11),
    MAZE_ENTRY_FLAG_12 = (1 << 12),
    MAZE_ENTRY_FLAG_13 = (1 << 13),
    MAZE_ENTRY_FLAG_14 = (1 << 14),
    MAZE_ENTRY_FLAG_15 = (1 << 15),
};

constexpr uint32_t kFloorSpritesCork[] = {
    SPR_FLOOR_CORK_SE_SW,
    SPR_FLOOR_CORK_SW,
    SPR_FLOOR_CORK_SE,
    SPR_FLOOR_CORK,
};
constexpr uint32_t kFloorSpritesMetal[] = {
    SPR_FLOOR_METAL,
    SPR_FLOOR_METAL,
    SPR_FLOOR_METAL,
    SPR_FLOOR_METAL,
};
constexpr uint32_t kFloorSpritesMetalB[] = {
    SPR_FLOOR_METAL_B,
    SPR_FLOOR_METAL_B,
    SPR_FLOOR_METAL_B,
    SPR_FLOOR_METAL_B,
};

constexpr uint32_t kFenceSpritesRope[] = {
    SPR_FENCE_ROPE_NE,
    SPR_FENCE_ROPE_SE,
    SPR_FENCE_ROPE_SW,
    SPR_FENCE_ROPE_NW,
};
constexpr uint32_t kFenceSpritesMetal[] = {
    SPR_FENCE_METAL_NE,
    SPR_FENCE_METAL_SE,
    SPR_FENCE_METAL_SW,
    SPR_FENCE_METAL_NW,
};
constexpr uint32_t kFenceSpritesMetalB[] = {
    SPR_FENCE_METAL_B_NE,
    SPR_FENCE_METAL_B_SE,
    SPR_FENCE_METAL_B_SW,
    SPR_FENCE_METAL_B_NW,
};

constexpr uint32_t kTrackSpritesSubmarineRideMiniHelicoptersQuarterTurn3Tiles[4][3] = {
    {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2,
    },
    {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2,
    },
    {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2,
    },
    {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2,
    }
};
constexpr uint32_t kTrackSpritesSubmarineRideMiniHelicoptersQuarterTurn1Tile[4] = {
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_SW_NW,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_NW_NE,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_NE_SE,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_SE_SW,
};

extern const uint8_t kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[];
extern const Direction kMapReversedDiagonalStraight[];

extern const CoordsXY kDefaultRightQuarterTurn5TilesOffsets[4][5];
extern const CoordsXYZ kDefaultRightQuarterTurn5TilesBoundOffsets[4][5];
extern const CoordsXY kDefaultRightQuarterTurn5TilesBoundLengths[4][5];

extern const uint8_t kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[];
extern const CoordsXY defaultRightQuarterTurn3TilesOffsets[4][3];
extern const CoordsXYZ defaultRightQuarterTurn3TilesBoundOffsets[4][3];
extern const CoordsXY defaultRightQuarterTurn3TilesBoundLengths[4][3];

constexpr int8_t kRightQuarterTurn3TilesSpriteMap[] = { 0, -1, 1, 2 };

extern const CoordsXY defaultRightHelixUpSmallQuarterBoundLengths[4][3][2];
extern const CoordsXYZ defaultRightHelixUpSmallQuarterBoundOffsets[4][3][2];

extern const CoordsXY defaultRightHelixUpLargeQuarterBoundLengths[4][5][2];
extern const CoordsXYZ defaultRightHelixUpLargeQuarterBoundOffsets[4][5][2];

extern const CoordsXYZ defaultLeftEighthToDiagBoundOffsets[4][4];
extern const CoordsXY defaultLeftEighthToDiagBoundLengths[4][4];
extern const CoordsXYZ defaultRightEighthToDiagBoundOffsets[4][4];
extern const CoordsXY defaultRightEighthToDiagBoundLengths[4][4];
extern const int8_t defaultEighthToDiagThickness[4][4];

extern const CoordsXY defaultDiagBoundLengths[4];
extern const CoordsXY defaultDiagTileOffsets[4];

constexpr bool kDiagSpriteMap[kNumOrthogonalDirections][4] = {
    { false, true, false, false },
    { false, false, false, true },
    { false, false, true, false },
    { true, false, false, false },
};

extern const uint8_t mapLeftEighthTurnToOrthogonal[5];

ImageId GetStationColourScheme(PaintSession& session, const TrackElement& trackElement);
ImageId GetShopSupportColourScheme(PaintSession& session, const TrackElement& trackElement);

bool TrackPaintUtilHasFence(
    enum edge_t edge, const CoordsXY& position, const TrackElement& trackElement, const Ride& ride, uint8_t rotation);
void TrackPaintUtilPaintFloor(
    PaintSession& session, uint8_t edges, ImageId colourFlags, uint16_t height, const uint32_t floorSprites[4],
    const StationObject* stationStyle);
void TrackPaintUtilPaintFences(
    PaintSession& session, uint8_t edges, const CoordsXY& position, const TrackElement& trackElement, const Ride& ride,
    const ImageId colourFlags, uint16_t height, const uint32_t fenceSprites[4], uint8_t rotation);
bool TrackPaintUtilDrawStationCovers(
    PaintSession& session, enum edge_t edge, bool hasFence, const StationObject* stationObject, uint16_t height,
    ImageId colour);
bool TrackPaintUtilDrawStationCovers2(
    PaintSession& session, enum edge_t edge, bool hasFence, const StationObject* stationObject, uint16_t height,
    uint8_t stationVariant, ImageId colour);
void TrackPaintUtilDrawNarrowStationPlatform(
    PaintSession& session, const Ride& ride, Direction direction, int32_t height, int32_t zOffset,
    const TrackElement& trackElement);
void TrackPaintUtilDrawStation(
    PaintSession& session, const Ride& ride, Direction direction, uint16_t height, const TrackElement& trackElement);
void TrackPaintUtilDrawStation2(
    PaintSession& session, const Ride& ride, Direction direction, uint16_t height, const TrackElement& trackElement,
    int32_t fenceOffsetA, int32_t fenceOffsetB);
void TrackPaintUtilDrawStation3(
    PaintSession& session, const Ride& ride, Direction direction, uint16_t height, uint16_t coverHeight,
    const TrackElement& trackElement);
void TrackPaintUtilDrawStationInverted(
    PaintSession& session, const Ride& ride, Direction direction, int32_t height, const TrackElement& trackElement,
    uint8_t stationVariant);
bool TrackPaintUtilShouldPaintSupports(const CoordsXY& position);
void TrackPaintUtilDrawPier(
    PaintSession& session, const Ride& ride, const StationObject* stationObject, const CoordsXY& position, Direction direction,
    int32_t height, const TrackElement& trackElement, uint8_t rotation);
inline void TrackPaintUtilDrawStationTunnel(PaintSession& session, Direction direction, int32_t height)
{
    PaintUtilPushTunnelRotated(session, direction, height, TunnelGroup::Square, TunnelSubType::Flat);
}
inline void TrackPaintUtilDrawStationTunnelTall(PaintSession& session, Direction direction, int32_t height)
{
    PaintUtilPushTunnelRotated(session, direction, height, TunnelGroup::Square, TunnelSubType::Tall);
}

void TrackPaintUtilRightQuarterTurn5TilesPaint(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][5], const CoordsXY offsets[4][5], const CoordsXY boundsLengths[4][5],
    const CoordsXYZ boundsOffsets[4][5]);
void TrackPaintUtilRightQuarterTurn5TilesPaint2(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, const ImageId colourFlags,
    const SpriteBb sprites[][5]);
void TrackPaintUtilRightQuarterTurn5TilesPaint3(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, const ImageId colourFlags,
    const SpriteBb sprites[][5]);

void TrackPaintUtilRightQuarterTurn3TilesPaint(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3], const CoordsXY offsets[4][3], const CoordsXY boundsLengths[4][3],
    const CoordsXYZ boundsOffsets[4][3]);
void TrackPaintUtilRightQuarterTurn3TilesPaint2(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3]);
void TrackPaintUtilRightQuarterTurn3TilesPaint2WithHeightOffset(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3], int32_t heightOffset);
void TrackPaintUtilRightQuarterTurn3TilesPaint3(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, const ImageId colourFlags,
    const SpriteBb sprites[4][3]);
void TrackPaintUtilRightQuarterTurn3TilesPaint4(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, const ImageId colourFlags,
    const SpriteBb sprites[4][3]);

void TrackPaintUtilLeftQuarterTurn3TilesPaint(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3]);
void TrackPaintUtilLeftQuarterTurn3TilesPaintWithHeightOffset(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3], int32_t heightOffset);
void TrackPaintUtilLeftQuarterTurn1TilePaint(
    PaintSession& session, int8_t thickness, int16_t height, int16_t boundBoxZOffset, Direction direction,
    const ImageId colourFlags, const uint32_t* sprites);
void TrackPaintUtilSpinningTunnelPaint(PaintSession& session, int8_t thickness, int16_t height, Direction direction);

/**
 * Renders the black metal platform and the supports of the on-ride photo section.
 *
 * @param session
 * @param direction
 * @param height
 * @param supportType
 */
void TrackPaintUtilOnridePhotoPlatformPaint(
    PaintSession& session, Direction direction, int32_t height, MetalSupportType supportType);

void TrackPaintUtilOnridePhotoSmallPaint(
    PaintSession& session, Direction direction, int32_t height, const TrackElement& trackElement);
void TrackPaintUtilOnridePhotoPaint(
    PaintSession& session, Direction direction, int32_t height, const TrackElement& trackElement);
void TrackPaintUtilOnridePhotoPaint2(
    PaintSession& session, Direction direction, int32_t height, int32_t trackHeightOffset, int32_t supportsAboveHeightOffset,
    const TrackElement& trackElement);
void TrackPaintUtilOnridePhotoPaint2(
    PaintSession& session, Direction direction, const TrackElement& trackElement, int32_t height,
    int32_t supportsAboveHeightOffset = kGeneralSupportHeightOnRidePhoto, int32_t trackHeightOffset = 3);
void TrackPaintUtilRightHelixUpSmallQuarterTilesPaint(
    PaintSession& session, const int8_t thickness[2], int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3][2], const CoordsXY offsets[4][3][2],
    const CoordsXY boundsLengths[4][3][2], const CoordsXYZ boundsOffsets[4][3][2]);
void TrackPaintUtilRightHelixUpLargeQuarterTilesPaint(
    PaintSession& session, const int8_t thickness[2], int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][5][2], const CoordsXY offsets[4][5][2],
    const CoordsXY boundsLengths[4][5][2], const CoordsXYZ boundsOffsets[4][5][2]);
void TrackPaintUtilEighthToDiagTilesPaint(
    PaintSession& session, const int8_t thickness[4][4], int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][4], const CoordsXY offsets[4][4], const CoordsXY boundsLengths[4][4],
    const CoordsXYZ boundsOffsets[4][4]);
void TrackPaintUtilDiagTilesPaint(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const uint32_t sprites[4], const CoordsXY offsets[4], const CoordsXY boundsLengths[4], const CoordsXYZ boundsOffsets[4],
    int8_t additionalBoundsHeight, const ImageId colourFlags);
inline void TrackPaintUtilDiagTilesPaint(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const uint32_t sprites[4], const CoordsXY offsets[4], const CoordsXY boundsLengths[4],
    const CoordsXYZ boundsOffsets[4] = nullptr, int8_t additionalBoundsHeight = 0)
{
    TrackPaintUtilDiagTilesPaint(
        session, thickness, height, direction, trackSequence, sprites, offsets, boundsLengths, boundsOffsets,
        additionalBoundsHeight, session.TrackColours);
}

void TrackPaintUtilDiagTilesPaintExtra(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const uint32_t sprites[4], MetalSupportType supportType);

void TrackPaintUtilRightVerticalLoopSegments(PaintSession& session, Direction direction, uint8_t trackSequence);

void TrackPaintUtilLeftCorkscrewUpSupports(PaintSession& session, Direction direction, uint16_t height);

void DrawSBendLeftSupports(
    PaintSession& session, MetalSupportType supportType, uint8_t sequence, Direction direction, int32_t height,
    int32_t specialA, int32_t specialB);
void DrawSBendRightSupports(
    PaintSession& session, MetalSupportType supportType, uint8_t sequence, Direction direction, int32_t height,
    int32_t specialA, int32_t specialB);

using TrackPaintFunction = void (&)(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType);

void TrackPaintFunctionDummy(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType);

TrackPaintFunction GetTrackPaintFunctionStandUpRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSuspendedSwingingRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionInvertedRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionJuniorRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMonorail(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMiniSuspendedRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMiniatureRailway(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionBoatHire(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionWoodenWildMouse(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSteeplechase(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionCarRide(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionLaunchedFreefall(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionBobsleighRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionObservationTower(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionLoopingRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionDinghySlide(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionDinghySlideCovered(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMineTrainRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionChairlift(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMaze(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSpiralSlide(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionGoKarts(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionLogFlume(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionRiverRapids(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionDodgems(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSwingingShip(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSwingingInverterShip(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionFerrisWheel(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMotionsimulator(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunction3dCinema(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionTopspin(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSpaceRings(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionReverseFreefallRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionLift(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionShop(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMerryGoRound(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionFacility(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionTwist(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionHauntedHouse(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionCircus(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionGhostTrain(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSideFrictionRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionWoodenRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionWildMouse(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMultiDimensionRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionFlyingRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionFlyingRCInverted(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionVirginiaReel(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSplashBoats(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMiniHelicopters(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionLayDownRCInverted(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSuspendedMonorail(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionReverserRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionHeartlineTwisterRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMiniGolf(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionLatticeTriangleTrack(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionLatticeTriangleTrackAlt(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionRotoDrop(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionFlyingSaucers(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionCrookedHouse(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMonorailCycles(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionCompactInvertedRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionWaterRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionAirPoweredVerticalRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionInvertedHairpinRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMagicCarpet(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionSubmarineRide(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionEnterprise(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionInvertedImpulseRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMiniRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionMineRide(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionLimLaunchedRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionTwisterRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionCorkscrewRC(OpenRCT2::TrackElemType trackType);
namespace OpenRCT2::HybridRC
{
    TrackPaintFunction GetTrackPaintFunction(OpenRCT2::TrackElemType trackType);
}
namespace OpenRCT2::SingleRailRC
{
    TrackPaintFunction GetTrackPaintFunction(OpenRCT2::TrackElemType trackType);
}
namespace OpenRCT2::AlpineRC
{
    TrackPaintFunction GetTrackPaintFunction(OpenRCT2::TrackElemType trackType);
}
TrackPaintFunction GetTrackPaintFunctionClassicWoodenRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionClassicStandUpRC(OpenRCT2::TrackElemType trackType);
TrackPaintFunction GetTrackPaintFunctionClassicWoodenTwisterRC(OpenRCT2::TrackElemType trackType);
