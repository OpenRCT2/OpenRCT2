/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "TileElement.h"

enum
{
    TERRAIN_GRASS,
    TERRAIN_SAND,
    TERRAIN_DIRT,
    TERRAIN_ROCK,
    TERRAIN_MARTIAN,
    TERRAIN_CHECKERBOARD,
    TERRAIN_GRASS_CLUMPS,
    TERRAIN_ICE,
    TERRAIN_GRID_RED,
    TERRAIN_GRID_YELLOW,
    TERRAIN_GRID_BLUE,
    TERRAIN_GRID_GREEN,
    TERRAIN_SAND_DARK,
    TERRAIN_SAND_LIGHT,

    TERRAIN_RCT2_COUNT,

    TERRAIN_ROOF_BROWN = TERRAIN_RCT2_COUNT,
    TERRAIN_ROOF_GREY,
    TERRAIN_ROOF_IRON,
    TERRAIN_ROOF_LOG,
    TERRAIN_COUNT_REGULAR, // The amount of surface types the user can actually select - what follows are technical types
    // Technical types (not selectable)
    TERRAIN_CHECKERBOARD_INVERTED = TERRAIN_COUNT_REGULAR,
    TERRAIN_UNDERGROUND_VIEW,
};

enum
{
    TERRAIN_EDGE_ROCK,
    TERRAIN_EDGE_WOOD_RED,
    TERRAIN_EDGE_WOOD_BLACK,
    TERRAIN_EDGE_ICE,

    TERRAIN_EDGE_RCT2_COUNT,

    TERRAIN_EDGE_BRICK = TERRAIN_EDGE_RCT2_COUNT,
    TERRAIN_EDGE_IRON,
    TERRAIN_EDGE_GREY,
    TERRAIN_EDGE_YELLOW,
    TERRAIN_EDGE_RED,
    TERRAIN_EDGE_PURPLE,
    TERRAIN_EDGE_GREEN,
    TERRAIN_EDGE_STONE_BROWN,
    TERRAIN_EDGE_STONE_GREY,
    TERRAIN_EDGE_SKYSCRAPER_A,
    TERRAIN_EDGE_SKYSCRAPER_B,

    TERRAIN_EDGE_COUNT
};

enum
{
    GRASS_LENGTH_MOWED,
    GRASS_LENGTH_CLEAR_0,
    GRASS_LENGTH_CLEAR_1,
    GRASS_LENGTH_CLEAR_2,
    GRASS_LENGTH_CLUMPS_0,
    GRASS_LENGTH_CLUMPS_1,
    GRASS_LENGTH_CLUMPS_2
};

enum
{
    OWNERSHIP_UNOWNED = 0,
    OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED = (1 << 4),
    OWNERSHIP_OWNED = (1 << 5),
    OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE = (1 << 6),
    OWNERSHIP_AVAILABLE = (1 << 7)
};

constexpr uint8_t kTileElementSurfaceOwnershipMask = 0xF0;
constexpr uint8_t kTileElementSurfaceParkFenceMask = 0x0F;
