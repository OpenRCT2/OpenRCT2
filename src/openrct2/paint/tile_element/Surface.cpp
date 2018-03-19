#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include <cstring>

#include "../../OpenRCT2.h"
#include "../../Cheats.h"
#include "../../config/Config.h"
#include "../../core/Math.hpp"
#include "../../core/Util.hpp"
#include "../../drawing/Drawing.h"
#include "../../interface/Colour.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../peep/Staff.h"
#include "../../ride/TrackDesign.h"
#include "../../sprites.h"
#include "../../world/Sprite.h"
#include "Surface.h"
#include "TileElement.h"

// clang-format off
static constexpr const uint8 byte_97B444[] =
{
    0, 2, 1, 3, 8, 10, 9, 11, 4, 6,
    5, 7, 12, 14, 13, 15, 0, 0, 0, 0,
    0, 0, 0, 17, 0, 0, 0, 16, 0, 18,
    15, 0
};

// rct2: 0x97B464, 0x97B474, 0x97B484, 0x97B494
static constexpr const LocationXY16 viewport_surface_paint_data[][4] = {
    {
        { 32, 0 },
        { -32, 32 },
        { -64, -32 },
        { 0, -64 }
    },
    {
        { 0, 32 },
        { -64, 0 },
        { -32, -64 },
        { 32, -32 }
    },
    {
        { 0, -32 },
        { 0, 0 },
        { -32, 0 },
        { -32, -32 }
    },
    {
        { -32, 0 },
        { -32, -32 },
        { 0, -32 },
        { 0, 0 }
    }
};

enum
{
    CORNER_TOP,
    CORNER_RIGHT,
    CORNER_BOTTOM,
    CORNER_LEFT
};

struct corner_height
{
    uint8 top;
    uint8 right;
    uint8 bottom;
    uint8 left;
};

/**
* rct2: 0x0097B4A4 (R), 0x0097B4C4 (T), 0x0097B4E4 (L), 0x0097B504 (B)
*/
static constexpr const corner_height corner_heights[] = {
    //   T  R  B  L
    { 0, 0, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 },
    { 0, 0, 1, 1 },
    { 1, 0, 0, 0 },
    { 1, 0, 1, 0 },
    { 1, 0, 0, 1 },
    { 1, 0, 1, 1 },
    { 0, 1, 0, 0 },
    { 0, 1, 1, 0 },
    { 0, 1, 0, 1 },
    { 0, 1, 1, 1 },
    { 1, 1, 0, 0 },
    { 1, 1, 1, 0 },
    { 1, 1, 0, 1 },
    { 1, 1, 1, 1 },
    { 0, 0, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 },
    { 0, 0, 1, 1 },
    { 1, 0, 0, 0 },
    { 1, 0, 1, 0 },
    { 1, 0, 0, 1 },
    { 1, 0, 1, 2 },
    { 0, 1, 0, 0 },
    { 0, 1, 1, 0 },
    { 0, 1, 0, 1 },
    { 0, 1, 2, 1 },
    { 1, 1, 0, 0 },
    { 1, 2, 1, 0 },
    { 2, 1, 0, 1 },
    { 1, 1, 1, 1 },
};

// bottom left tint
static constexpr const uint8 byte_97B524[] = {
    2, 5, 1, 4, 2, 5, 1, 2, 2, 4,
    1, 2, 1, 3, 0, 3, 1, 5, 0
};

// top left tint
static constexpr const uint32 byte_97B537[] = {
    2, 5, 2, 4, 2, 5, 1, 1, 3, 4,
    3, 2, 1, 2, 0, 3, 1, 5, 0
};

// top right tint
static constexpr const uint8 byte_97B54A[] = {
    2, 2, 2, 4, 0, 0, 1, 1, 3, 4,
    3, 5, 1, 2, 2, 3, 1, 5, 0
};

// bottom right tint
static constexpr const uint8 byte_97B55D[] = {
    2, 2, 1, 4, 0, 0, 1, 2, 2, 4,
    1, 5, 1, 3, 2, 3, 1, 5, 0
};

static constexpr const uint8 stru_97B570[][2] = {
    { 2, 2 },
    { 3, 3 },
    { 3, 5 },
    { 3, 3 },
    { 4, 4 },
    { 4, 6 },
    { 2, 2 },
    { 3, 3 },
    { 3, 5 },
    { 3, 3 },
    { 2, 3 },
    { 2, 3 },
    { 2, 3 },
    { 3, 4 },
    { 2, 3 },
    { 3, 4 }
};

static constexpr const sint16 word_97B590[] = {
    0,
    0,
    -32,
    0,
    0,
    -48,
    0,
    0,
    -32,
    0,
    -16,
    -16,
    -16,
    -16,
    -16,
    -16,
};

// tunnel offset
static constexpr const uint8 byte_97B5B0[] = {
    0, 0, 0, 3, 3, 3, 6, 6, 6, 6,
    10, 11, 12, 13, 14, 14
};

#define DEFINE_EDGE_SPRITES(base) { \
    (base) +  0, \
    (base) + 20, \
    (base) + 10, \
    (base) + 30, \
}
#define DEFINE_EDGE_TUNNEL_SPRITES(base) { \
    (base) + 36, \
    (base) + 40, \
    (base) + 44, \
    (base) + 48, \
    (base) + 52, \
    (base) + 56, \
    (base) + 60, \
    (base) + 64, \
    (base) + 68, \
    (base) + 72, \
    (base) + 76, \
    (base) + 80, \
    (base) + 36, \
    (base) + 48, \
    (base) + 60, \
    (base) + 72, \
}

static constexpr const uint32 _terrainEdgeSpriteIds[][TERRAIN_EDGE_COUNT] =
{
    DEFINE_EDGE_SPRITES(SPR_EDGE_ROCK_BASE),
    DEFINE_EDGE_SPRITES(SPR_EDGE_WOOD_RED_BASE),
    DEFINE_EDGE_SPRITES(SPR_EDGE_WOOD_BLACK_BASE),
    DEFINE_EDGE_SPRITES(SPR_EDGE_ICE_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_BRICK_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_IRON_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_GREY_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_YELLOW_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_RED_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_PURPLE_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_GREEN_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_STONE_BROWN_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_STONE_GREY_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_SKYSCRAPER_A_BASE),
    DEFINE_EDGE_SPRITES(SPR_CSG_EDGE_SKYSCRAPER_B_BASE),
};

static constexpr const uint32 _terrainEdgeTunnelSpriteIds[][TERRAIN_EDGE_COUNT * 4] =
{
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_EDGE_ROCK_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_EDGE_WOOD_RED_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_EDGE_WOOD_BLACK_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_EDGE_ICE_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_BRICK_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_IRON_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_GREY_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_YELLOW_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_RED_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_PURPLE_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_GREEN_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_STONE_BROWN_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_STONE_GREY_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_SKYSCRAPER_A_BASE),
    DEFINE_EDGE_TUNNEL_SPRITES(SPR_CSG_EDGE_SKYSCRAPER_B_BASE),
};

static constexpr const uint8 byte_97B740[] =
{
    0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
    0, 3, 0, 1, 4, 0
};

static constexpr const uint32 dword_97B750[][2] =
{
    { SPR_TERRAIN_GRASS,                                          SPR_TERRAIN_GRASS_GRID },
    { SPR_TERRAIN_SAND_YELLOW,                                    SPR_TERRAIN_SAND_YELLOW_GRID },
    { SPR_TERRAIN_DIRT,                                           SPR_TERRAIN_DIRT_GRID },
    { SPR_TERRAIN_ROCK,                                           SPR_TERRAIN_ROCK_GRID },
    { SPR_TERRAIN_MARTIAN,                                        SPR_TERRAIN_MARTIAN_GRID },
    { SPR_TERRAIN_CHECKERBOARD,                                   SPR_TERRAIN_CHECKERBOARD_GRID },
    { SPR_TERRAIN_GRASS_CLUMPS,                                   SPR_TERRAIN_GRASS_CLUMPS_GRID },
    { SPR_TERRAIN_ICE,                                            SPR_TERRAIN_ICE_GRID },
    { SPR_TERRAIN_GRID | COLOUR_BRIGHT_RED << 19 | IMAGE_TYPE_REMAP,    SPR_TERRAIN_GRID_GRID | COLOUR_BRIGHT_RED << 19 | IMAGE_TYPE_REMAP },
    { SPR_TERRAIN_GRID | COLOUR_YELLOW << 19 | IMAGE_TYPE_REMAP,        SPR_TERRAIN_GRID_GRID | COLOUR_YELLOW << 19 | IMAGE_TYPE_REMAP },
    { SPR_TERRAIN_GRID | COLOUR_BRIGHT_PURPLE << 19 | IMAGE_TYPE_REMAP, SPR_TERRAIN_GRID_GRID | COLOUR_BRIGHT_PURPLE << 19 | IMAGE_TYPE_REMAP },
    { SPR_TERRAIN_GRID | COLOUR_BRIGHT_GREEN << 19 | IMAGE_TYPE_REMAP,  SPR_TERRAIN_GRID_GRID | COLOUR_BRIGHT_GREEN << 19 | IMAGE_TYPE_REMAP },
    { SPR_TERRAIN_SAND_RED,                                       SPR_TERRAIN_SAND_RED_GRID },
    { SPR_TERRAIN_SAND,                                           SPR_TERRAIN_SAND_GRID },
    { SPR_TERRAIN_CHECKERBOARD_INVERTED,                          SPR_TERRAIN_CHECKERBOARD_INVERTED_GRID },
};

static constexpr const uint32 dword_97B7C8[] =
{
    SPR_TERRAIN_GRASS_UNDERGROUND,
    SPR_TERRAIN_SAND_YELLOW_UNDERGROUND,
    SPR_TERRAIN_DIRT_UNDERGROUND,
    SPR_TERRAIN_ROCK_UNDERGROUND,
    SPR_TERRAIN_MARTIAN_UNDERGROUND,
    SPR_TERRAIN_CHECKERBOARD_UNDERGROUND,
    SPR_TERRAIN_GRASS_CLUMPS_UNDERGROUND,
    SPR_TERRAIN_ICE_UNDERGROUND,
    SPR_TERRAIN_GRID_UNDERGROUND | COLOUR_BRIGHT_RED << 19 | IMAGE_TYPE_REMAP,
    SPR_TERRAIN_GRID_UNDERGROUND | COLOUR_YELLOW << 19 | IMAGE_TYPE_REMAP,
    SPR_TERRAIN_GRID_UNDERGROUND | COLOUR_BRIGHT_PURPLE << 19 | IMAGE_TYPE_REMAP,
    SPR_TERRAIN_GRID_UNDERGROUND | COLOUR_BRIGHT_GREEN << 19 | IMAGE_TYPE_REMAP,
    SPR_TERRAIN_SAND_RED_UNDERGROUND,
    SPR_TERRAIN_SAND_UNDERGROUND,
    SPR_TERRAIN_CHECKERBOARD_INVERTED_UNDERGROUND,
};

static constexpr const uint32 dword_97B804[] =
{
    SPR_TERRAIN_PATTERN_GRASS,
    SPR_TERRAIN_PATTERN_SAND_YELLOW,
    SPR_TERRAIN_PATTERN_DIRT,
    SPR_TERRAIN_PATTERN_ROCK,
    SPR_TERRAIN_PATTERN_MARTIAN,
    SPR_TERRAIN_PATTERN_GRASS,
    SPR_TERRAIN_PATTERN_GRASS_CLUMPS,
    SPR_TERRAIN_PATTERN_ICE,
    SPR_TERRAIN_PATTERN_GRASS,
    SPR_TERRAIN_PATTERN_GRASS,
    SPR_TERRAIN_PATTERN_GRASS,
    SPR_TERRAIN_PATTERN_GRASS,
    SPR_TERRAIN_PATTERN_SAND_RED,
    SPR_TERRAIN_PATTERN_SAND
};

enum
{
    FLAG_DONT_SMOOTHEN = (1 << 0),
    FLAG_DONT_SMOOTHEN_SELF = (1 << 1),
};

static constexpr const uint8 byte_97B83C[] =
{
    0,
    0,
    0,
    FLAG_DONT_SMOOTHEN_SELF,
    FLAG_DONT_SMOOTHEN_SELF,
    FLAG_DONT_SMOOTHEN_SELF | FLAG_DONT_SMOOTHEN,
    0,
    0,
    FLAG_DONT_SMOOTHEN_SELF | FLAG_DONT_SMOOTHEN,
    FLAG_DONT_SMOOTHEN_SELF | FLAG_DONT_SMOOTHEN,
    FLAG_DONT_SMOOTHEN_SELF | FLAG_DONT_SMOOTHEN,
    FLAG_DONT_SMOOTHEN_SELF | FLAG_DONT_SMOOTHEN,
    0,
    0
};

static constexpr const uint8 byte_97B84A[] =
{
    0, 1, 2, 3, 4, 14, 6, 7, 8, 9,
    10, 11, 12, 13
};

static constexpr const uint32 dword_97B858[][2] =
{
    { SPR_TERRAIN_GRASS_LENGTH_4_VARIANT_1, SPR_TERRAIN_GRASS_LENGTH_4_VARIANT_1_GRID },
    { SPR_TERRAIN_GRASS_LENGTH_4_VARIANT_2, SPR_TERRAIN_GRASS_LENGTH_4_VARIANT_2_GRID },
    { SPR_TERRAIN_GRASS_LENGTH_4_VARIANT_3, SPR_TERRAIN_GRASS_LENGTH_4_VARIANT_3_GRID },
    { SPR_TERRAIN_GRASS_LENGTH_4_VARIANT_4, SPR_TERRAIN_GRASS_LENGTH_4_VARIANT_4_GRID },
};

static constexpr const uint32 dword_97B878[][2] =
{
    { SPR_TERRAIN_GRASS_LENGTH_6_VARIANT_1, SPR_TERRAIN_GRASS_LENGTH_6_VARIANT_1_GRID },
    { SPR_TERRAIN_GRASS_LENGTH_6_VARIANT_2, SPR_TERRAIN_GRASS_LENGTH_6_VARIANT_2_GRID },
    { SPR_TERRAIN_GRASS_LENGTH_6_VARIANT_3, SPR_TERRAIN_GRASS_LENGTH_6_VARIANT_3_GRID },
    { SPR_TERRAIN_GRASS_LENGTH_6_VARIANT_4, SPR_TERRAIN_GRASS_LENGTH_6_VARIANT_4_GRID },
};

static constexpr const uint32 dword_97B898[][2] =
{
    { SPR_TERRAIN_GRASS_MOWED_90, SPR_TERRAIN_GRASS_MOWED_90_GRID },
    { SPR_TERRAIN_GRASS_MOWED,    SPR_TERRAIN_GRASS_MOWED_GRID },
    { SPR_TERRAIN_GRASS_MOWED_90, SPR_TERRAIN_GRASS_MOWED_90_GRID },
    { SPR_TERRAIN_GRASS_MOWED,    SPR_TERRAIN_GRASS_MOWED_GRID }
};

struct tile_descriptor
{
    const rct_tile_element * tile_element;
    uint8 terrain;
    uint8 slope;
    corner_height corner_heights;
};

struct tile_surface_boundary_data
{
    sint32       bit_1;
    sint32       bit_8;
    sint32       bit_4;
    sint32       bit_2;
    uint32       image[5];
    LocationXY8  offset;
    LocationXY16 box_offset;
    LocationXY16 box_size;
};

static constexpr const tile_surface_boundary_data _tileSurfaceBoundaries[4] =
{
    { // Bottom right
        1, 8, 4, 2,
        {
            SPR_TERRAIN_BOUNDARY_FENCES_1,
            SPR_TERRAIN_BOUNDARY_FENCES_5,
            SPR_TERRAIN_BOUNDARY_FENCES_3,
            SPR_TERRAIN_BOUNDARY_FENCES_3,
            SPR_TERRAIN_BOUNDARY_FENCES_5,
        },
        { 1, 31 },
        { 1,  31 },
        { 30,  1 }
    },
    { // Bottom left
        1, 2, 4, 8,
        {
            SPR_TERRAIN_BOUNDARY_FENCES_2,
            SPR_TERRAIN_BOUNDARY_FENCES_6,
            SPR_TERRAIN_BOUNDARY_FENCES_4,
            SPR_TERRAIN_BOUNDARY_FENCES_4,
            SPR_TERRAIN_BOUNDARY_FENCES_6,
        },
        { 31, 0 },
        { 31,  1 },
        { 1,  30 }
    },
    { // Top left
        4, 2, 8, 1,
        {
            SPR_TERRAIN_BOUNDARY_FENCES_1,
            SPR_TERRAIN_BOUNDARY_FENCES_3,
            SPR_TERRAIN_BOUNDARY_FENCES_5,
            SPR_TERRAIN_BOUNDARY_FENCES_3,
            SPR_TERRAIN_BOUNDARY_FENCES_5,
        },
        { 1, 0 },
        { 1,  1 },
        { 30,  1 }
    },
    { // Top right
        4, 8, 2, 1,
        {
            SPR_TERRAIN_BOUNDARY_FENCES_2,
            SPR_TERRAIN_BOUNDARY_FENCES_4,
            SPR_TERRAIN_BOUNDARY_FENCES_6,
            SPR_TERRAIN_BOUNDARY_FENCES_4,
            SPR_TERRAIN_BOUNDARY_FENCES_6,
        },
        { 1, 1 },
        { 1,  1 },
        { 1,  30 }
    },
};
// clang-format on

static uint8 viewport_surface_paint_setup_get_relative_slope(const rct_tile_element * tileElement, sint32 rotation)
{
    const uint8 slope = tileElement->properties.surface.slope;
    const uint8 slopeHeight = slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
    uint16 slopeCorners = (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) << rotation;
    slopeCorners = ((slopeCorners >> 4) | slopeCorners) & 0x0F;
    return slopeHeight | slopeCorners;
}

/**
* rct2: 0x0065E890, 0x0065E946, 0x0065E9FC, 0x0065EAB2
*/
static void viewport_surface_smoothen_edge(paint_session * session, enum edge_t edge, struct tile_descriptor self, struct tile_descriptor neighbour)
{
    if (neighbour.tile_element == nullptr)
        return;

    uint32 maskImageBase = 0;
    uint8 neighbourCorners[2] = { 0 };
    uint8 ownCorners[2] = { 0 };

    switch (edge)
    {
    case EDGE_BOTTOMLEFT:
        maskImageBase = SPR_TERRAIN_EDGE_MASK_BOTTOM_LEFT;
        neighbourCorners[0] = neighbour.corner_heights.top;
        neighbourCorners[1] = neighbour.corner_heights.right;
        ownCorners[0] = self.corner_heights.left;
        ownCorners[1] = self.corner_heights.bottom;
        break;
    case EDGE_BOTTOMRIGHT:
        maskImageBase = SPR_TERRAIN_EDGE_MASK_BOTTOM_RIGHT;
        neighbourCorners[0] = neighbour.corner_heights.top;
        neighbourCorners[1] = neighbour.corner_heights.left;
        ownCorners[0] = self.corner_heights.right;
        ownCorners[1] = self.corner_heights.bottom;
        break;
    case EDGE_TOPLEFT:
        maskImageBase = SPR_TERRAIN_EDGE_MASK_TOP_LEFT;
        neighbourCorners[0] = neighbour.corner_heights.right;
        neighbourCorners[1] = neighbour.corner_heights.bottom;
        ownCorners[0] = self.corner_heights.top;
        ownCorners[1] = self.corner_heights.left;
        break;
    case EDGE_TOPRIGHT:
        maskImageBase = SPR_TERRAIN_EDGE_MASK_TOP_RIGHT;
        neighbourCorners[0] = neighbour.corner_heights.left;
        neighbourCorners[1] = neighbour.corner_heights.bottom;
        ownCorners[0] = self.corner_heights.top;
        ownCorners[1] = self.corner_heights.right;
        break;
    }

    if (ownCorners[0] != neighbourCorners[0] ||
        ownCorners[1] != neighbourCorners[1])
    {
        // Only smoothen tiles that align
        return;
    }

    uint8 dh = 0, cl = 0;
    switch (edge)
    {
    case EDGE_BOTTOMLEFT:
        dh = byte_97B524[byte_97B444[self.slope]];
        cl = byte_97B54A[byte_97B444[neighbour.slope]];
        break;

    case EDGE_TOPLEFT:
        dh = byte_97B537[byte_97B444[self.slope]];
        cl = byte_97B55D[byte_97B444[neighbour.slope]];
        break;

    case EDGE_BOTTOMRIGHT:
        dh = byte_97B55D[byte_97B444[self.slope]];
        cl = byte_97B537[byte_97B444[neighbour.slope]];
        break;

    case EDGE_TOPRIGHT:
        dh = byte_97B54A[byte_97B444[self.slope]];
        cl = byte_97B524[byte_97B444[neighbour.slope]];
        break;
    }

    if (self.terrain == neighbour.terrain)
    {
        // same tint
        if (cl == dh)
            return;

        if (byte_97B83C[self.terrain] & FLAG_DONT_SMOOTHEN_SELF)
            return;
    }
    else
    {
        if (byte_97B83C[self.terrain] & FLAG_DONT_SMOOTHEN)
            return;

        if (byte_97B83C[neighbour.terrain] & FLAG_DONT_SMOOTHEN)
            return;
    }

    const uint32 image_id = maskImageBase + byte_97B444[self.slope];

    if (paint_attach_to_previous_ps(session, image_id, 0, 0))
    {
        attached_paint_struct * out = session->UnkF1AD2C;
        // set content and enable masking
        out->colour_image_id = dword_97B804[neighbour.terrain] + cl;
        out->flags |= PAINT_STRUCT_FLAG_IS_MASKED;
    }
}

static void viewport_surface_draw_tile_side_bottom(paint_session * session, enum edge_t edge, uint8 height, uint8 edgeStyle, struct tile_descriptor self, struct tile_descriptor neighbour, bool isWater)
{
    if (!is_csg_loaded() && edgeStyle >= TERRAIN_EDGE_RCT2_COUNT)
        edgeStyle = TERRAIN_EDGE_ROCK;

    sint16 al, ah, cl, ch, dl = 0, dh;

    LocationXY8 offset = { 0, 0 };
    LocationXY8 bounds = { 0, 0 };
    LocationXY16 tunnelBounds = { 1, 1 };
    LocationXY16 tunnelTopBoundBoxOffset = { 0, 0 };

    tunnel_entry * tunnelArray;
    switch (edge)
    {
    case EDGE_BOTTOMLEFT:
        al = self.corner_heights.left;
        cl = self.corner_heights.bottom;

        ah = neighbour.corner_heights.top;
        ch = neighbour.corner_heights.right;

        offset.x = 30;
        bounds.y = 30;
        tunnelBounds.x = 32;
        tunnelTopBoundBoxOffset.y = 31;

        tunnelArray = session->LeftTunnels;
        break;

    case EDGE_BOTTOMRIGHT:
        al = self.corner_heights.right;
        cl = self.corner_heights.bottom;

        ah = neighbour.corner_heights.top;
        ch = neighbour.corner_heights.left;

        offset.y = 30;
        bounds.x = 30;
        tunnelBounds.y = 32;
        tunnelTopBoundBoxOffset.x = 31;

        tunnelArray = session->RightTunnels;
        break;

    default:
        return;
    }

    if (isWater)
        dl = height;

    if (neighbour.tile_element == nullptr)
    {
        ch = 1;
        ah = 1;
    }
    else
    {
        if (isWater)
        {
            dh = map_get_water_height(neighbour.tile_element);
            if (dl == dh)
            {
                return;
            }

            al = dl;
            cl = dl;
        }
    }

    if (al <= ah && cl <= ch)
    {
        return;
    }

    if (!is_csg_loaded() && edgeStyle >= TERRAIN_EDGE_RCT2_COUNT)
        edgeStyle = TERRAIN_EDGE_ROCK;

    uint32 base_image_id = _terrainEdgeSpriteIds[edgeStyle][0];
    if (gCurrentViewportFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
    {
        base_image_id = _terrainEdgeSpriteIds[edgeStyle][1];
    }

    if (edge == EDGE_BOTTOMRIGHT)
    {
        base_image_id += 5;
    }

    uint8 curHeight = Math::Min(ah, ch);
    if (ch != ah)
    {
        // If bottom part of edge isn't straight, add a filler
        uint32 image_offset = 3;

        if (ch >= ah)
        {
            image_offset = 4;
        }

        if (curHeight != al && curHeight != cl)
        {
            uint32 image_id = base_image_id + image_offset;
            sub_98196C(session, image_id, offset.x, offset.y, bounds.x, bounds.y, 15, curHeight * 16);
            curHeight++;
        }
    }

    ah = cl;

    for(uint32 tunnelIndex = 0; tunnelIndex < TUNNEL_MAX_COUNT;)
    {
        if (curHeight >= al || curHeight >= cl)
        {
            // If top of edge isn't straight, add a filler
            uint32 image_offset = 1;
            if (curHeight >= al)
            {
                image_offset = 2;

                if (curHeight >= cl)
                {
                    return;
                }
            }

            const uint32 image_id = base_image_id + image_offset;
            sub_98196C(session, image_id, offset.x, offset.y, bounds.x, bounds.y, 15, curHeight * 16);

            return;
        }

        if (curHeight != tunnelArray[tunnelIndex].height)
        {
            // Normal walls
            while (curHeight > tunnelArray[tunnelIndex].height)
            {
                tunnelIndex++;
            }

            if (isWater == true || curHeight != tunnelArray[tunnelIndex].height)
            {
                sub_98196C(session, base_image_id, offset.x, offset.y, bounds.x, bounds.y, 15, curHeight * 16);

                curHeight++;
                continue;
            }
        }

        // Tunnels
        uint8 tunnelType = tunnelArray[tunnelIndex].type;
        uint8 tunnelHeight = stru_97B570[tunnelType][0];
        sint16 zOffset = curHeight;

        if ((zOffset + tunnelHeight) > ah || (zOffset + tunnelHeight) > al)
        {
            tunnelType = byte_97B5B0[tunnelType];
        }

        zOffset *= 16;

        sint16 boundBoxOffsetZ = zOffset + word_97B590[tunnelType];
        sint8 boundBoxLength = stru_97B570[tunnelType][1] * 16;
        if (boundBoxOffsetZ < 16)
        {
            boundBoxOffsetZ += 16;
            boundBoxLength -= 16;
        }

        uint32 image_id = _terrainEdgeTunnelSpriteIds[edgeStyle][tunnelType] + (edge == EDGE_BOTTOMRIGHT ? 2 : 0);
        sub_98197C(
            session, image_id, offset.x, offset.y, tunnelBounds.x, tunnelBounds.y, boundBoxLength - 1, zOffset, 0, 0,
            boundBoxOffsetZ);

        boundBoxOffsetZ = curHeight * 16;
        boundBoxLength = stru_97B570[tunnelType][1] * 16;
        boundBoxOffsetZ += word_97B590[tunnelType];
        if (boundBoxOffsetZ == 0)
        {
            boundBoxOffsetZ += 16;
            boundBoxLength -= 16;
        }

        image_id = _terrainEdgeTunnelSpriteIds[edgeStyle][tunnelType] + (edge == EDGE_BOTTOMRIGHT ? 2 : 0) + 1;
        sub_98197C(
            session, image_id, offset.x, offset.y, tunnelBounds.x, tunnelBounds.y, boundBoxLength - 1, curHeight * 16,
            tunnelTopBoundBoxOffset.x, tunnelTopBoundBoxOffset.y, boundBoxOffsetZ);

        curHeight += stru_97B570[tunnelType][0];
        tunnelIndex++;
    }
}

/**
* rct2: 0x0065EB7D, 0x0065F0D8
*/
static void viewport_surface_draw_land_side_bottom(paint_session * session, enum edge_t edge, uint8 height, uint8 edgeStyle, struct tile_descriptor self, struct tile_descriptor neighbour)
{
    viewport_surface_draw_tile_side_bottom(session, edge, height, edgeStyle, self, neighbour, false);
}

/**
* rct2: 0x0065F8B9, 0x0065FE26
*/
static void viewport_surface_draw_water_side_bottom(paint_session * session, enum edge_t edge, uint8 height, uint8 edgeStyle, struct tile_descriptor self, struct tile_descriptor neighbour)
{
    viewport_surface_draw_tile_side_bottom(session, edge, height, edgeStyle, self, neighbour, true);
}

static void viewport_surface_draw_tile_side_top(paint_session * session, enum edge_t edge, uint8 height, uint8 terrain, struct tile_descriptor self, struct tile_descriptor neighbour, bool isWater)
{
    if (!is_csg_loaded() && terrain >= TERRAIN_EDGE_RCT2_COUNT)
        terrain = TERRAIN_EDGE_ROCK;

    sint16 al, ah, cl, ch, dl = 0, dh;

    LocationXY8 offset = { 0, 0 };
    LocationXY8 bounds = { 0, 0 };

    switch (edge)
    {
    case EDGE_TOPLEFT:
        al = self.corner_heights.top;
        cl = self.corner_heights.left;

        ah = neighbour.corner_heights.right;
        ch = neighbour.corner_heights.bottom;

        offset.y = -2;
        bounds.x = 30;
        break;

    case EDGE_TOPRIGHT:
        al = self.corner_heights.top;
        cl = self.corner_heights.right;

        ah = neighbour.corner_heights.left;
        ch = neighbour.corner_heights.bottom;

        offset.x = -2;
        bounds.y = 30;
        break;

    default:
        return;
    }

    if(isWater == false)
        dl = height;

    // save ecx
    if (neighbour.tile_element == nullptr)
    {
        ah = 1;
        ch = 1;
    }
    else
    {
        if (isWater)
        {
            dh = map_get_water_height(neighbour.tile_element);
            if (dl == dh)
            {
                return;
            }

            al = dl;
            cl = dl;
        }
    }

    // al + cl probably are self tile corners, while ah/ch are neighbour tile corners
    if (al <= ah && cl <= ch)
    {
        return;
    }

    uint32 base_image_id;

    if (isWater)
    {
        base_image_id = _terrainEdgeSpriteIds[terrain][2]; // var_08
        if (gCurrentViewportFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
        {
            base_image_id = _terrainEdgeSpriteIds[terrain][1];  // var_04
        }
        base_image_id += (edge == EDGE_TOPLEFT ? 5 : 0);
    }
    else
    {
        if (!(gCurrentViewportFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE))
        {
            const uint8 incline = (cl - al) + 1;
            const uint32 image_id = _terrainEdgeSpriteIds[terrain][3] + (edge == EDGE_TOPLEFT ? 3 : 0) + incline; // var_c;
            const sint16 y = (dl - al) * 16;
            paint_attach_to_previous_ps(session, image_id, 0, y);
            return;
        }
        base_image_id = _terrainEdgeSpriteIds[terrain][1] + (edge == EDGE_TOPLEFT ? 5 : 0); // var_04
    }

    uint8 cur_height = Math::Min(ch, ah);
    if (ch != ah)
    {
        // neighbour tile corners aren't level
        uint32 image_offset = 3;
        if (ch > ah)
        {
            image_offset = 4;
        }

        if (cur_height != al && cur_height != cl)
        {
            const uint32 image_id = base_image_id + image_offset;
            sub_98196C(session, image_id, offset.x, offset.y, bounds.x, bounds.y, 15, cur_height * 16);
            cur_height++;
        }
    }

    ah = cl;

    if (isWater)
    {
        offset.xy = 0;
    }

    while (cur_height < al && cur_height < ah)
    {
        sub_98196C(session, base_image_id, offset.x, offset.y, bounds.x, bounds.y, 15, cur_height * 16);
        cur_height++;
    }

    uint32 image_offset = 1;
    if (cur_height >= al)
    {
        image_offset = 2;

        if (cur_height >= ah)
        {
            return;
        }
    }

    const uint32 image_id = base_image_id + image_offset;
    sub_98196C(session, image_id, offset.x, offset.y, bounds.x, bounds.y, 15, cur_height * 16);
}

/**
* rct2: 0x0065F63B, 0x0065F77D
*/
static void viewport_surface_draw_land_side_top(paint_session * session, enum edge_t edge, uint8 height, uint8 terrain, struct tile_descriptor self, struct tile_descriptor neighbour)
{
    viewport_surface_draw_tile_side_top(session, edge, height, terrain, self, neighbour, false);
}

/**
* rct2: 0x0066039B, 0x006604F1
*/
static void viewport_surface_draw_water_side_top(paint_session * session, enum edge_t edge, uint8 height, uint8 terrain, struct tile_descriptor self, struct tile_descriptor neighbour)
{
    viewport_surface_draw_tile_side_top(session, edge, height, terrain, self, neighbour, true);
}

/**
* rct2: 0x0066062C
*
* @param direction (cl)
* @param height (dx)
* @param tile_element (esi)
*/
void surface_paint(paint_session * session, uint8 direction, uint16 height, const rct_tile_element * tileElement)
{
    rct_drawpixelinfo * dpi = session->Unk140E9A8;
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_TERRAIN;
    session->DidPassSurface = true;
    session->SurfaceElement = tileElement;

    const uint16 zoomLevel = dpi->zoom_level;
    const uint8 rotation = session->CurrentRotation;
    const uint32 terrain_type = tile_element_get_terrain(tileElement);
    const uint8 surfaceShape = viewport_surface_paint_setup_get_relative_slope(tileElement, rotation);
    const LocationXY16& base = session->SpritePosition;
    const corner_height& cornerHeights = corner_heights[surfaceShape];

    tile_descriptor selfDescriptor =
    {
        tileElement,
        (uint8)terrain_type,
        surfaceShape,
        {
            (uint8)(height / 16 + cornerHeights.top),
            (uint8)(height / 16 + cornerHeights.right),
            (uint8)(height / 16 + cornerHeights.bottom),
            (uint8)(height / 16 + cornerHeights.left),
        }
    };

    tile_descriptor tileDescriptors[5];
    tileDescriptors[0] = selfDescriptor;

    for (sint32 i = 0; i < 4; i++)
    {
        const LocationXY16& offset = viewport_surface_paint_data[i][rotation];
        const CoordsXY position =
        {
            (sint32)(base.x + offset.x),
            (sint32)(base.y + offset.y)
        };

        tile_descriptor& descriptor = tileDescriptors[i + 1];

        descriptor.tile_element = nullptr;
        if (position.x > 0x2000 || position.y > 0x2000)
        {
            continue;
        }

        rct_tile_element * surfaceElement = map_get_surface_element_at(position);
        if (surfaceElement == nullptr)
        {
            continue;
        }

        const uint32 surfaceSlope = viewport_surface_paint_setup_get_relative_slope(surfaceElement, rotation);
        const uint8 baseHeight = surfaceElement->base_height / 2;
        const corner_height& ch = corner_heights[surfaceSlope];

        descriptor.tile_element = surfaceElement;
        descriptor.terrain = tile_element_get_terrain(surfaceElement);
        descriptor.slope = surfaceSlope;
        descriptor.corner_heights.top = baseHeight + ch.top;
        descriptor.corner_heights.right = baseHeight + ch.right;
        descriptor.corner_heights.bottom = baseHeight + ch.bottom;
        descriptor.corner_heights.left = baseHeight + ch.left;
    }


    if ((gCurrentViewportFlags & VIEWPORT_FLAG_LAND_HEIGHTS) && (zoomLevel == 0))
    {
        const sint16 x = session->MapPosition.x;
        const sint16 y = session->MapPosition.y;

        sint32 dx = tile_element_height(x + 16, y + 16) & 0xFFFF;
        dx += 3;

        sint32 image_id = (SPR_HEIGHT_MARKER_BASE + dx / 16) | 0x20780000;
        image_id += get_height_marker_offset();
        image_id -= gMapBaseZ;

        sub_98196C(session, image_id, 16, 16, 1, 1, 0, height);
    }

    bool has_surface = false;
    if (session->VerticalTunnelHeight * 16 == height)
    {
        // Vertical tunnels
        sub_98197C(session, 1575, 0, 0, 1, 30, 39, height, -2, 1, height - 40);
        sub_98197C(session, 1576, 0, 0, 30, 1, 0, height, 1, 31, height);
        sub_98197C(session, 1577, 0, 0, 1, 30, 0, height, 31, 1, height);
        sub_98197C(session, 1578, 0, 0, 30, 1, 39, height, 1, -2, height - 40);
    }
    else
    {
        const bool showGridlines = (gCurrentViewportFlags & VIEWPORT_FLAG_GRIDLINES);

        sint32 branch = -1;
        if ((tileElement->properties.surface.terrain & 0xE0) == 0)
        {
            if (tile_element_get_direction(tileElement) == 0)
            {
                if (zoomLevel == 0)
                {
                    if ((gCurrentViewportFlags & (VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_UNDERGROUND_INSIDE)) == 0)
                    {
                        branch = tileElement->properties.surface.grass_length & 0x7;
                    }
                }
            }
        }

        assert(surfaceShape < Util::CountOf(byte_97B444));
        const uint8 image_offset = byte_97B444[surfaceShape];
        sint32 image_id;
        uint32 ebp = terrain_type;

        switch (branch)
        {
        case 0:
            // loc_660C90
            image_id = dword_97B898[rotation][showGridlines ? 1 : 0] + image_offset;
            break;

        case 1:
        case 2:
        case 3:
        default:
            // loc_660C9F
            if (rotation & 1)
            {
                assert(ebp < Util::CountOf(byte_97B84A));
                ebp = byte_97B84A[ebp];
            }
            assert(ebp < Util::CountOf(dword_97B750));
            image_id = dword_97B750[ebp][showGridlines ? 1 : 0] + image_offset;

            if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
            {
                image_id = SPR_TERRAIN_TRACK_DESIGNER;
            }

            if (gCurrentViewportFlags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE))
            {
                image_id &= 0xDC07FFFF; // remove colour
                image_id |= 0x41880000;
            }
            break;

        case 4:
        case 5:
            // loc_660C44
        case 6:
            // loc_660C6A
            {
                const sint16 x = session->MapPosition.x & 0x20;
                const sint16 y = session->MapPosition.y & 0x20;
                const sint32 index = (y | (x << 1)) >> 5;

                if (branch == 6)
                {
                    image_id = dword_97B878[index][showGridlines ? 1 : 0] + image_offset;
                }
                else
                {
                    image_id = dword_97B858[index][showGridlines ? 1 : 0] + image_offset;
                }
            }
            break;
        }

        sub_98196C(session, image_id, 0, 0, 32, 32, -1, height);
        has_surface = true;
    }

    // Draw Staff Patrol Areas
    // loc_660D02
    if (gStaffDrawPatrolAreas != SPRITE_INDEX_NULL)
    {
        const sint32 staffIndex = gStaffDrawPatrolAreas;
        const bool is_staff_list = staffIndex & 0x8000;
        const sint16 x = session->MapPosition.x, y = session->MapPosition.y;

        uint8 staffType = staffIndex & 0x7FFF;
        uint32 image_id = IMAGE_TYPE_REMAP;
        uint8 patrolColour = COLOUR_LIGHT_BLUE;

        if (!is_staff_list)
        {
            rct_peep * staff = GET_PEEP(staffIndex);
            if (!staff_is_patrol_area_set(staff->staff_id, x, y))
            {
                patrolColour = COLOUR_GREY;
            }
            staffType = staff->staff_type;
        }

        if (staff_is_patrol_area_set(200 + staffType, x, y))
        {
            assert(surfaceShape < Util::CountOf(byte_97B444));

            image_id |= SPR_TERRAIN_SELECTION_PATROL_AREA + byte_97B444[surfaceShape];
            image_id |= patrolColour << 19;
            paint_attach_to_previous_ps(session, image_id, 0, 0);
        }
    }

    // Draw Peep Spawns
    if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) &&
        gCurrentViewportFlags & VIEWPORT_FLAG_LAND_OWNERSHIP)
    {
        const LocationXY16& pos = session->MapPosition;
        for (auto &spawn : gPeepSpawns)
        {
            if ((spawn.x & 0xFFE0) == pos.x && (spawn.y & 0xFFE0) == pos.y)
            {
                sub_98196C(session, SPR_TERRAIN_SELECTION_SQUARE_SIMPLE, 0, 0, 32, 32, 16, spawn.z);

                const sint32 offset = ((spawn.direction ^ 2) + rotation) & 3;
                const uint32 image_id = (PEEP_SPAWN_ARROW_0 + offset) | 0x20380000;
                sub_98196C(session, image_id, 0, 0, 32, 32, 19, spawn.z);
            }
        }
    }

    if (gCurrentViewportFlags & VIEWPORT_FLAG_LAND_OWNERSHIP)
    {
        // loc_660E9A:
        if (tileElement->properties.surface.ownership & OWNERSHIP_OWNED)
        {
            assert(surfaceShape < Util::CountOf(byte_97B444));
            paint_attach_to_previous_ps(session, SPR_TERRAIN_SELECTION_SQUARE + byte_97B444[surfaceShape], 0, 0);
        }
        else if (tileElement->properties.surface.ownership & OWNERSHIP_AVAILABLE)
        {
            const LocationXY16& pos = session->MapPosition;
            const sint32 height2 = (tile_element_height(pos.x + 16, pos.y + 16) & 0xFFFF) + 3;
            paint_struct * backup = session->UnkF1AD28;
            sub_98196C(session, SPR_LAND_OWNERSHIP_AVAILABLE, 16, 16, 1, 1, 0, height2);
            session->UnkF1AD28 = backup;
        }
    }

    if (gCurrentViewportFlags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS &&
        !(tileElement->properties.surface.ownership & OWNERSHIP_OWNED))
    {
        if (tileElement->properties.surface.ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
        {
            assert(surfaceShape < Util::CountOf(byte_97B444));
            paint_attach_to_previous_ps(session, SPR_TERRAIN_SELECTION_DOTTED + byte_97B444[surfaceShape], 0, 0);
        }
        else if (tileElement->properties.surface.ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE)
        {
            const LocationXY16& pos = session->MapPosition;
            const sint32 height2 = tile_element_height(pos.x + 16, pos.y + 16) & 0xFFFF;
            paint_struct * backup = session->UnkF1AD28;
            sub_98196C(session, SPR_LAND_CONSTRUCTION_RIGHTS_AVAILABLE, 16, 16, 1, 1, 0, height2 + 3);
            session->UnkF1AD28 = backup;
        }
    }

    // ebx[0] = esi;
    // ebp[4] = ebp;
    // ebp[8] = ebx

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
    {
        // loc_660FB8:
        const LocationXY16& pos = session->MapPosition;
        if (pos.x >= gMapSelectPositionA.x &&
            pos.x <= gMapSelectPositionB.x &&
            pos.y >= gMapSelectPositionA.y &&
            pos.y <= gMapSelectPositionB.y)
        {
            const uint16 mapSelectionType = gMapSelectType;
            if (mapSelectionType >= MAP_SELECT_TYPE_EDGE_0)
            {
                // Walls
                // loc_661089:
                const uint32 eax = ((((mapSelectionType - 9) + rotation) & 3) + 0x21) << 19;
                const uint32 image_id = (SPR_TERRAIN_SELECTION_EDGE + byte_97B444[surfaceShape]) | eax | IMAGE_TYPE_REMAP;
                paint_attach_to_previous_ps(session, image_id, 0, 0);
            }
            else if (mapSelectionType >= MAP_SELECT_TYPE_QUARTER_0)
            {
                // loc_661051:(no jump)
                // Selection split into four quarter segments
                const uint32 eax = ((((mapSelectionType - MAP_SELECT_TYPE_QUARTER_0) + rotation) & 3) + 0x27) << 19;
                const uint32 image_id = (SPR_TERRAIN_SELECTION_QUARTER + byte_97B444[surfaceShape]) | eax | IMAGE_TYPE_REMAP;
                paint_attach_to_previous_ps(session, image_id, 0, 0);
            }
            else if (mapSelectionType <= MAP_SELECT_TYPE_FULL)
            {
                // Corners
                uint32 eax = mapSelectionType;
                if (mapSelectionType != MAP_SELECT_TYPE_FULL)
                {
                    eax = (mapSelectionType + rotation) & 3;
                }

                eax = (eax + 0x21) << 19;
                const uint32 image_id = (SPR_TERRAIN_SELECTION_CORNER + byte_97B444[surfaceShape]) | eax | IMAGE_TYPE_REMAP;
                paint_attach_to_previous_ps(session, image_id, 0, 0);
            }
            else
            {
                sint32 local_surfaceShape = surfaceShape;
                sint32 local_height = height;
                // Water tool
                if (map_get_water_height(tileElement) > 0)
                {
                    sint32 waterHeight = map_get_water_height(tileElement) * 16;
                    if (waterHeight > height)
                    {
                        local_height += 16;

                        if (waterHeight != local_height
                            || !(local_surfaceShape & 0x10))
                        {
                            local_height = waterHeight;
                            local_surfaceShape = 0;
                        }
                        else
                        {
                            sint16 bl, bh;

                            bl = (surfaceShape ^ 0xF) << 2;
                            bh = bl >> 4;
                            local_surfaceShape = (bh & 0x3) | (bl & 0xC);
                        }
                    }
                }

                const sint32 image_id = (SPR_TERRAIN_SELECTION_CORNER + byte_97B444[local_surfaceShape]) | 0x21300000;

                paint_struct * backup = session->UnkF1AD28;
                sub_98196C(session, image_id, 0, 0, 32, 32, 1, local_height);
                session->UnkF1AD28 = backup;
            }
        }
    }

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        const LocationXY16& pos = session->MapPosition;

        for (const LocationXY16 * tile = gMapSelectionTiles; tile->x != -1; tile++)
        {
            if (tile->x != pos.x || tile->y != pos.y)
            {
                continue;
            }

            uint32 colours = COLOUR_GREY << 24 | COLOUR_BRIGHT_PURPLE << 19;
            if (gMapSelectFlags & MAP_SELECT_FLAG_GREEN)
            {
                colours = COLOUR_GREY << 24 | COLOUR_SATURATED_GREEN << 19;
            }

            const uint32 image_id = (SPR_TERRAIN_SELECTION_CORNER + byte_97B444[surfaceShape]) | colours | IMAGE_TYPE_REMAP;
            paint_attach_to_previous_ps(session, image_id, 0, 0);
            break;
        }
    }

    if (zoomLevel == 0 &&
        has_surface &&
        !(gCurrentViewportFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE) &&
        !(gCurrentViewportFlags & VIEWPORT_FLAG_HIDE_BASE) &&
        gConfigGeneral.landscape_smoothing)
    {
        viewport_surface_smoothen_edge(session, EDGE_TOPLEFT, tileDescriptors[0], tileDescriptors[3]);
        viewport_surface_smoothen_edge(session, EDGE_TOPRIGHT, tileDescriptors[0], tileDescriptors[4]);
        viewport_surface_smoothen_edge(session, EDGE_BOTTOMLEFT, tileDescriptors[0], tileDescriptors[1]);
        viewport_surface_smoothen_edge(session, EDGE_BOTTOMRIGHT, tileDescriptors[0], tileDescriptors[2]);
    }


    if ((gCurrentViewportFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE) &&
        !(gCurrentViewportFlags & VIEWPORT_FLAG_HIDE_BASE) &&
        !(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        const uint8 image_offset = byte_97B444[surfaceShape];
        uint32 base_image = terrain_type;
        if (rotation & 1) {
            base_image = byte_97B84A[terrain_type];
        }
        const uint32 image_id = dword_97B7C8[base_image] + image_offset;
        paint_attach_to_previous_ps(session, image_id, 0, 0);
    }

    if (!(gCurrentViewportFlags & VIEWPORT_FLAG_HIDE_VERTICAL))
    {
        // loc_66122C:
        const uint8 al_edgeStyle = tileElement->properties.surface.slope & TILE_ELEMENT_SURFACE_EDGE_STYLE_MASK;
        const uint8 di_type = tileElement->type & 0x80;

        const uint32 eax = al_edgeStyle + di_type * 2;
        if (eax != 32 && eax != 0 && eax != 96 && eax != 64)
        {
            log_verbose("eax: %d", eax);
        }

        tunnel_entry backupLeftTunnels[TUNNEL_MAX_COUNT];
        tunnel_entry backupRightTunnels[TUNNEL_MAX_COUNT];

        memcpy(backupLeftTunnels, session->LeftTunnels, sizeof(tunnel_entry) * TUNNEL_MAX_COUNT);
        memcpy(backupRightTunnels, session->RightTunnels, sizeof(tunnel_entry) * TUNNEL_MAX_COUNT);

        viewport_surface_draw_land_side_top(session, EDGE_TOPLEFT, height / 16, eax / 32, tileDescriptors[0], tileDescriptors[3]);
        viewport_surface_draw_land_side_top(session, EDGE_TOPRIGHT, height / 16, eax / 32, tileDescriptors[0], tileDescriptors[4]);
        viewport_surface_draw_land_side_bottom(session, EDGE_BOTTOMLEFT, height / 16, eax / 32, tileDescriptors[0], tileDescriptors[1]);
        viewport_surface_draw_land_side_bottom(session, EDGE_BOTTOMRIGHT, height / 16, eax / 32, tileDescriptors[0], tileDescriptors[2]);

        memcpy(session->LeftTunnels, backupLeftTunnels, sizeof(tunnel_entry) * TUNNEL_MAX_COUNT);
        memcpy(session->RightTunnels, backupRightTunnels, sizeof(tunnel_entry) * TUNNEL_MAX_COUNT);
    }

    if (map_get_water_height(tileElement) > 0)
    {
        // loc_6615A9: (water height)
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_WATER;

        const uint16 localHeight = height + 16;
        const uint16 waterHeight = map_get_water_height(tileElement) * 16;

        if (!gTrackDesignSaveMode)
        {
            session->WaterHeight = waterHeight;

            sint32 image_offset = 0;
            if (waterHeight <= localHeight)
            {
                image_offset = byte_97B740[surfaceShape & 0xF];
            }

            const sint32 image_id = (SPR_WATER_MASK + image_offset) | IMAGE_TYPE_REMAP | IMAGE_TYPE_TRANSPARENT | PALETTE_WATER << 19;
            sub_98196C(session, image_id, 0, 0, 32, 32, -1, waterHeight);

            paint_attach_to_previous_ps(session, SPR_WATER_OVERLAY + image_offset, 0, 0);

            // This wasn't in the original, but the code depended on globals that were only set in a different conditional
            const uint8 al_edgeStyle = tileElement->properties.surface.slope & TILE_ELEMENT_SURFACE_EDGE_STYLE_MASK;
            const uint8 di_type = tileElement->type & 0x80;
            const uint32 eax = al_edgeStyle + di_type * 2;
            assert(eax % 32 == 0);
            // end new code

            viewport_surface_draw_water_side_top(session, EDGE_TOPLEFT, waterHeight / 16, eax / 32, tileDescriptors[0], tileDescriptors[3]);
            viewport_surface_draw_water_side_top(session, EDGE_TOPRIGHT, waterHeight / 16, eax / 32, tileDescriptors[0], tileDescriptors[4]);
            viewport_surface_draw_water_side_bottom(session, EDGE_BOTTOMLEFT, waterHeight / 16, eax / 32, tileDescriptors[0], tileDescriptors[1]);
            viewport_surface_draw_water_side_bottom(session, EDGE_BOTTOMRIGHT, waterHeight / 16, eax / 32, tileDescriptors[0], tileDescriptors[2]);
        }
    }

    if ((tileElement->properties.surface.ownership & 0x0F) && !gTrackDesignSaveMode)
    {
        // Owned land boundary fences
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_PARK;

        registers regs = { 0 };
        regs.al = tileElement->properties.surface.ownership & 0x0F;
        regs.ax = regs.ax << rotation;
        regs.ah = regs.al >> 4;

        uint8 al = regs.al | regs.ah;

        for (const auto& fenceData : _tileSurfaceBoundaries)
        {
            const sint32 bit = al & 1;
            al >>= 1;

            if (bit == 0)
                continue;

            sint32 local_height = height;
            sint32 image_id = 0;

            if (!(surfaceShape & fenceData.bit_1))
            { // first
                if (surfaceShape & fenceData.bit_8)
                { // second
                    image_id = fenceData.image[2];
                }
                else
                {
                    image_id = fenceData.image[0];
                }
            }
            else if (!(surfaceShape & fenceData.bit_8))
            { // loc_6619A2:
                image_id = fenceData.image[1];
            }
            else
            {
                local_height += 16;

                if (!(surfaceShape & 0x10))
                { // loc_6619B5 (first)
                    image_id = fenceData.image[0];
                }
                else if (surfaceShape & fenceData.bit_4)
                { // loc_6619B5 (second)
                    image_id = fenceData.image[3];
                }
                else if (surfaceShape & fenceData.bit_2)
                { // loc_6619B5 (third)
                    image_id = fenceData.image[4];
                }
                else
                {
                    image_id = fenceData.image[0];
                }
            }

            sub_98197C(
                session, image_id, fenceData.offset.x, fenceData.offset.y, fenceData.box_size.x, fenceData.box_size.y, 9,
                local_height, fenceData.box_offset.x, fenceData.box_offset.y, local_height + 1);
        }
    }

    session->InteractionType = VIEWPORT_INTERACTION_ITEM_TERRAIN;
    session->Unk141E9DB |= G141E9DB_FLAG_1;

    switch (surfaceShape)
    {
    default:
        // loc_661C2C
        //     00
        //   00  00
        // 00  00  00
        //   00  00
        //     00
        paint_util_set_segment_support_height(session,
            SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
            height,
            0
        );
        paint_util_force_set_general_support_height(session, height, 0);
        break;

    case 1:
        // loc_661CB9
        //     00
        //   00  00
        // 01  01  01
        //   1B  1B
        //     1B
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, height, 0);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height, 1);
        paint_util_set_segment_support_height(session, SEGMENT_D0 | SEGMENT_D4, height + 6, 0x1B);
        paint_util_set_segment_support_height(session, SEGMENT_C0, height + 6 + 6, 0x1B);
        paint_util_force_set_general_support_height(session, height, 1);
        break;

    case 2:
        // loc_661D4E
        //     02
        //   17  00
        // 17  02  00
        //   17  00
        //     02
        paint_util_set_segment_support_height(session, SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, height, 0);
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height, 2);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_D0, height + 6, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_B8, height + 6 + 6, 0x17);
        paint_util_force_set_general_support_height(session, height, 2);
        break;

    case 3:
        // loc_661DE3
        //     03
        //   03  03
        // 03  03  03
        //   03  03
        //     03
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC, height + 2, 3);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4, height + 2 + 6, 3);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0, height + 2 + 6 + 6, 3);
        paint_util_force_set_general_support_height(session, height, 3);
        break;

    case 4:
        // loc_661E7C
        //     1E
        //   1E  1E
        // 04  04  04
        //   00  00
        //     00
        paint_util_set_segment_support_height(session, SEGMENT_C0 | SEGMENT_D0 | SEGMENT_D4, height, 0);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height, 4);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC, height + 6, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_B4, height + 6 + 6, 0x1E);
        paint_util_force_set_general_support_height(session, height, 4);
        break;

    case 5:
        // loc_661F11
        //     1E          ▓▓
        //   1E  1E      ▒▒  ▒▒
        // 05  05  05  ░░  ░░  ░░
        //   1B  1B      ▒▒  ▒▒
        //     1B          ▓▓
        paint_util_set_segment_support_height(session, SEGMENT_B4, height + 6 + 6, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC, height + 6, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height, 5);
        paint_util_set_segment_support_height(session, SEGMENT_D0 | SEGMENT_D4, height + 6, 0x1B);
        paint_util_set_segment_support_height(session, SEGMENT_C0, height + 6 + 6, 0x1B);
        paint_util_force_set_general_support_height(session, height, 5);
        break;

    case 6:
        // loc_661FA6
        //     06          ▓▓
        //   06  06      ▓▓  ▒▒
        // 06  06  06  ▓▓  ▒▒  ░░
        //   06  06      ▒▒  ░░
        //     06          ░░
        paint_util_set_segment_support_height(session, SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0, height + 2, 6);
        paint_util_set_segment_support_height(session, SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, height + 2 + 6, 6);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4, height + 2 + 6 + 6, 6);
        paint_util_force_set_general_support_height(session, height, 6);
        break;

    case 7:
        // loc_66203F
        //     07          ▓▓
        //   00  17      ▓▓  ▒▒
        // 00  07  17  ▓▓  ▓▓  ░░
        //   00  17      ▓▓  ▒▒
        //     07          ▓▓
        paint_util_set_segment_support_height(session, SEGMENT_BC, height + 4, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_CC | SEGMENT_D4, height + 4 + 6, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height + 4 + 6 + 6, 7);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_D0 | SEGMENT_B8, height + 4 + 6 + 6, 0);
        paint_util_force_set_general_support_height(session, height, 7);
        break;

    case 8:
        // loc_6620D8
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_C8 | SEGMENT_D0, height, 0);
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height, 8);
        paint_util_set_segment_support_height(session, SEGMENT_CC | SEGMENT_D4, height + 6, 0x1D);
        paint_util_set_segment_support_height(session, SEGMENT_BC, height + 6 + 6, 0x1D);
        paint_util_force_set_general_support_height(session, height, 8);
        break;

    case 9:
        // loc_66216D
        paint_util_force_set_general_support_height(session, height, 9);
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8, height + 2, 9);
        paint_util_set_segment_support_height(session, SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, height + 2 + 6, 9);
        paint_util_set_segment_support_height(session, SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC, height + 2 + 6 + 6, 9);
        break;

    case 10:
        // loc_662206
        paint_util_force_set_general_support_height(session, height, 0xA);
        paint_util_set_segment_support_height(session, SEGMENT_B8, height + 6 + 6, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_D0, height + 6, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height, 0xA);
        paint_util_set_segment_support_height(session, SEGMENT_CC | SEGMENT_D4, height + 6, 0x1D);
        paint_util_set_segment_support_height(session, SEGMENT_BC, height + 6 + 6, 0x1D);
        break;

    case 11:
        // loc_66229B
        paint_util_force_set_general_support_height(session, height, 0xB);
        paint_util_set_segment_support_height(session, SEGMENT_B4, height + 4, 0x1B);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC, height + 4 + 6, 0x1B);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height + 4 + 6 + 6, 0xB);
        paint_util_set_segment_support_height(session, SEGMENT_D0 | SEGMENT_D4 | SEGMENT_C0, height + 4 + 6 + 6, 0);
        break;

    case 12:
        // loc_662334
        paint_util_force_set_general_support_height(session, height, 0xC);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0, height + 2, 0xC);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4, height + 2 + 6, 0xC);
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC, height + 2 + 6 + 6, 0xC);
        break;

    case 13:
        // loc_6623CD
        paint_util_force_set_general_support_height(session, height, 0xD);
        paint_util_set_segment_support_height(session, SEGMENT_B8, height + 4, 0x1D);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_D0, height + 4 + 6, 0x1D);
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height + 4 + 6 + 6, 0xD);
        paint_util_set_segment_support_height(session, SEGMENT_CC | SEGMENT_D4 | SEGMENT_BC, height + 4 + 6 + 6, 0);
        break;

    case 14:
        // loc_662466
        paint_util_force_set_general_support_height(session, height, 0xE);
        paint_util_set_segment_support_height(session, SEGMENT_C0, height + 4, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_D0 | SEGMENT_D4, height + 4 + 6, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height + 4 + 6 + 6, 0xE);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC | SEGMENT_B4, height + 4 + 6 + 6, 0);
        break;

    case 23:
        // loc_6624FF
        paint_util_force_set_general_support_height(session, height, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_BC, height + 4, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_CC | SEGMENT_D4, height + 4 + 6, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height + 4 + 6 + 6, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_D0, height + 4 + 6 + 6 + 6, 0x17);
        paint_util_set_segment_support_height(session, SEGMENT_B8, height + 4 + 6 + 6 + 6 + 6, 0x17);
        break;

    case 27:
        // loc_6625A0
        paint_util_force_set_general_support_height(session, height, 0x1B);
        paint_util_set_segment_support_height(session, SEGMENT_B4, height + 4, 0x1B);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC, height + 4 + 6, 0x1B);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height + 4 + 6 + 6, 0x1B);
        paint_util_set_segment_support_height(session, SEGMENT_D0 | SEGMENT_D4, height + 4 + 6 + 6 + 6, 0x1B);
        paint_util_set_segment_support_height(session, SEGMENT_C0, height + 4 + 6 + 6 + 6 + 6, 0x1B);
        break;

    case 29:
        // loc_662641
        paint_util_force_set_general_support_height(session, height, 0x1D);
        paint_util_set_segment_support_height(session, SEGMENT_B8, height + 4, 0x1D);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_D0, height + 4 + 6, 0x1D);
        paint_util_set_segment_support_height(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height + 4 + 6 + 6, 0x1D);
        paint_util_set_segment_support_height(session, SEGMENT_CC | SEGMENT_D4, height + 4 + 6 + 6 + 6, 0x1D);
        paint_util_set_segment_support_height(session, SEGMENT_BC, height + 4 + 6 + 6 + 6 + 6, 0x1D);
        break;

    case 30:
        // loc_6626E2
        paint_util_force_set_general_support_height(session, height, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_C0, height + 4, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_D0 | SEGMENT_D4, height + 4 + 6, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height + 4 + 6 + 6, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC, height + 4 + 6 + 6 + 6, 0x1E);
        paint_util_set_segment_support_height(session, SEGMENT_B4, height + 4 + 6 + 6 + 6 + 6, 0x1E);
        break;
    }
}
