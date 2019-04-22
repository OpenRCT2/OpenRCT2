/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Surface.h"

#include "../../Cheats.h"
#include "../../Context.h"
#include "../../OpenRCT2.h"
#include "../../config/Config.h"
#include "../../core/Guard.hpp"
#include "../../drawing/Drawing.h"
#include "../../interface/Colour.h"
#include "../../interface/Viewport.h"
#include "../../object/ObjectManager.h"
#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../paint/Paint.h"
#include "../../peep/Staff.h"
#include "../../ride/TrackDesign.h"
#include "../../sprites.h"
#include "../../world/Sprite.h"
#include "../../world/Surface.h"
#include "Paint.TileElement.h"

#include <algorithm>
#include <cstring>
#include <iterator>

// clang-format off
static constexpr const uint8_t byte_97B444[] =
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
    uint8_t top;
    uint8_t right;
    uint8_t bottom;
    uint8_t left;
};

/**
*  rct2: 0x0097B4A4 (R), 0x0097B4C4 (T), 0x0097B4E4 (L), 0x0097B504 (B)
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
static constexpr const uint8_t byte_97B524[] = {
    2, 5, 1, 4, 2, 5, 1, 2, 2, 4,
    1, 2, 1, 3, 0, 3, 1, 5, 0
};

// top left tint
static constexpr const uint32_t byte_97B537[] = {
    2, 5, 2, 4, 2, 5, 1, 1, 3, 4,
    3, 2, 1, 2, 0, 3, 1, 5, 0
};

// top right tint
static constexpr const uint8_t byte_97B54A[] = {
    2, 2, 2, 4, 0, 0, 1, 1, 3, 4,
    3, 5, 1, 2, 2, 3, 1, 5, 0
};

// bottom right tint
static constexpr const uint8_t byte_97B55D[] = {
    2, 2, 1, 4, 0, 0, 1, 2, 2, 4,
    1, 5, 1, 3, 2, 3, 1, 5, 0
};

static constexpr const uint8_t _tunnelHeights[TUNNEL_TYPE_COUNT][2] = {
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
    { 3, 4 },
    { 2, 2 },
    { 2, 2 },
    { 2, 2 },
    { 2, 2 },
    { 2, 2 },
    { 2, 2 },
    { 2, 2 },
};

static constexpr const int16_t _boundBoxZOffsets[TUNNEL_TYPE_COUNT] = {
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
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

// tunnel offset
static constexpr const uint8_t byte_97B5B0[TUNNEL_TYPE_COUNT] = {
    0, 0, 0, 3, 3, 3, 6, 6, 6, 6,
    10, 11, 12, 13, 14, 14,
    16, 17, 18, 19, 20, 21, 22
};

static constexpr const uint8_t byte_97B740[] =
{
    0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
    0, 3, 0, 1, 4, 0
};

struct tile_descriptor
{
    TileCoordsXY tile_coords;
    const TileElement * tile_element;
    uint8_t terrain;
    uint8_t slope;
    corner_height corner_heights;
};

struct tile_surface_boundary_data
{
    int32_t       bit_1;
    int32_t       bit_8;
    int32_t       bit_4;
    int32_t       bit_2;
    uint32_t       image[5];
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

static const TerrainSurfaceObject* get_surface_object(size_t index)
{
    TerrainSurfaceObject* result{};
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_TERRAIN_SURFACE, index);
    if (obj != nullptr)
    {
        return static_cast<TerrainSurfaceObject*>(obj);
    }
    return result;
}

static uint32_t get_surface_image(
    const paint_session* session, uint8_t index, int32_t offset, uint8_t rotation, int32_t grassLength, bool grid,
    bool underground)
{
    auto image = (uint32_t)SPR_NONE;
    auto obj = get_surface_object(index);
    if (obj != nullptr)
    {
        image = obj->GetImageId(
            { session->MapPosition.x >> 5, session->MapPosition.y >> 5 }, grassLength, rotation, offset, grid, underground);
        if (obj->Colour != 255)
        {
            image |= obj->Colour << 19 | IMAGE_TYPE_REMAP;
        }
    }
    return image;
}

static uint32_t get_surface_pattern(uint8_t index, int32_t offset)
{
    auto image = (uint32_t)SPR_NONE;
    auto obj = get_surface_object(index);
    if (obj != nullptr)
    {
        image = obj->PatternBaseImageId + offset;
        if (obj->Colour != 255)
        {
            image |= obj->Colour << 19 | IMAGE_TYPE_REMAP;
        }
    }
    return image;
}

static bool surface_should_smooth_self(uint8_t index)
{
    auto obj = get_surface_object(index);
    if (obj != nullptr)
    {
        return obj->Flags & TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_SELF;
    }
    return false;
}

static bool surface_should_smooth(uint8_t index)
{
    auto obj = get_surface_object(index);
    if (obj != nullptr)
    {
        return obj->Flags & TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_OTHER;
    }
    return false;
}

static uint32_t get_edge_image_with_offset(uint8_t index, uint32_t offset)
{
    uint32_t result = 0;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_TERRAIN_EDGE, index);
    if (obj != nullptr)
    {
        auto tobj = static_cast<TerrainEdgeObject*>(obj);
        return tobj->BaseImageId + offset;
    }
    return result;
}

static uint32_t get_edge_image(uint8_t index, uint8_t type)
{
    static constexpr uint32_t offsets[] = {
        0,
        20,
        10,
        30,
    };

    uint32_t result = 0;
    if (type < std::size(offsets))
    {
        result = get_edge_image_with_offset(index, offsets[type]);
    }
    return result;
}

static uint32_t get_tunnel_image(uint8_t index, uint8_t type)
{
    static constexpr uint32_t offsets[] = {
        36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 36, 48, 60, 72,
    };

    uint32_t result = 0;
    if (type < std::size(offsets))
    {
        result = get_edge_image_with_offset(index, offsets[type]);
    }
    return result;
}

static uint8_t viewport_surface_paint_setup_get_relative_slope(const TileElement* tileElement, int32_t rotation)
{
    const uint8_t slope = tileElement->AsSurface()->GetSlope();
    const uint8_t slopeHeight = slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
    uint16_t slopeCorners = (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) << rotation;
    slopeCorners = ((slopeCorners >> 4) | slopeCorners) & 0x0F;
    return slopeHeight | slopeCorners;
}

/**
 *  rct2: 0x0065E890, 0x0065E946, 0x0065E9FC, 0x0065EAB2
 */
static void viewport_surface_smoothen_edge(
    paint_session* session, enum edge_t edge, struct tile_descriptor self, struct tile_descriptor neighbour)
{
    if (neighbour.tile_element == nullptr)
        return;

    uint32_t maskImageBase = 0;
    uint8_t neighbourCorners[2] = { 0 };
    uint8_t ownCorners[2] = { 0 };

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

    if (ownCorners[0] != neighbourCorners[0] || ownCorners[1] != neighbourCorners[1])
    {
        // Only smoothen tiles that align
        return;
    }

    uint8_t dh = 0, cl = 0;
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

        if (!surface_should_smooth_self(self.terrain))
            return;
    }
    else
    {
        if (!surface_should_smooth(self.terrain))
            return;

        if (!surface_should_smooth(neighbour.terrain))
            return;
    }

    const uint32_t image_id = maskImageBase + byte_97B444[self.slope];

    if (paint_attach_to_previous_ps(session, image_id, 0, 0))
    {
        attached_paint_struct* out = session->UnkF1AD2C;
        // set content and enable masking
        out->colour_image_id = get_surface_pattern(neighbour.terrain, cl);
        out->flags |= PAINT_STRUCT_FLAG_IS_MASKED;
    }
}

static bool tile_is_inside_clip_view(const tile_descriptor& tile)
{
    Guard::ArgumentNotNull(tile.tile_element);

    if (tile.tile_element->base_height > gClipHeight)
        return false;
    if (tile.tile_coords.x < gClipSelectionA.x || tile.tile_coords.x > gClipSelectionB.x)
        return false;
    if (tile.tile_coords.y < gClipSelectionA.y || tile.tile_coords.y > gClipSelectionB.y)
        return false;

    return true;
}

static void viewport_surface_draw_tile_side_bottom(
    paint_session* session, enum edge_t edge, uint8_t height, uint8_t edgeStyle, struct tile_descriptor self,
    struct tile_descriptor neighbour, bool isWater)
{
    int16_t cornerHeight1, neighbourCornerHeight1, cornerHeight2, neighbourCornerHeight2;

    LocationXY8 offset = { 0, 0 };
    LocationXY8 bounds = { 0, 0 };
    LocationXY16 tunnelBounds = { 1, 1 };
    LocationXY16 tunnelTopBoundBoxOffset = { 0, 0 };

    tunnel_entry* tunnelArray;
    switch (edge)
    {
        case EDGE_BOTTOMLEFT:
            cornerHeight1 = self.corner_heights.left;
            cornerHeight2 = self.corner_heights.bottom;

            neighbourCornerHeight1 = neighbour.corner_heights.top;
            neighbourCornerHeight2 = neighbour.corner_heights.right;

            offset.x = 30;
            bounds.y = 30;
            tunnelBounds.x = 32;
            tunnelTopBoundBoxOffset.y = 31;

            tunnelArray = session->LeftTunnels;
            break;

        case EDGE_BOTTOMRIGHT:
            cornerHeight1 = self.corner_heights.right;
            cornerHeight2 = self.corner_heights.bottom;

            neighbourCornerHeight1 = neighbour.corner_heights.top;
            neighbourCornerHeight2 = neighbour.corner_heights.left;

            offset.y = 30;
            bounds.x = 30;
            tunnelBounds.y = 32;
            tunnelTopBoundBoxOffset.x = 31;

            tunnelArray = session->RightTunnels;
            break;

        default:
            return;
    }

    bool neighbourIsClippedAway = (session->ViewFlags & VIEWPORT_FLAG_CLIP_VIEW) && !tile_is_inside_clip_view(neighbour);

    if (neighbour.tile_element == nullptr || neighbourIsClippedAway)
    {
        // The neighbour tile doesn't exist or isn't drawn - assume minimum height to draw full edges
        neighbourCornerHeight2 = MINIMUM_LAND_HEIGHT / 2;
        neighbourCornerHeight1 = MINIMUM_LAND_HEIGHT / 2;
    }

    if (isWater)
    {
        uint8_t waterHeight = neighbour.tile_element->AsSurface()->GetWaterHeight();
        if (waterHeight == height && !neighbourIsClippedAway)
        {
            // Don't draw the edge when the neighbour's water level is the same
            return;
        }

        cornerHeight1 = height;
        cornerHeight2 = height;
    }

    if (cornerHeight1 <= neighbourCornerHeight1 && cornerHeight2 <= neighbourCornerHeight2)
    {
        // The edge is not visible behind the neighbour's slope
        return;
    }

    if (!is_csg_loaded() && edgeStyle >= TERRAIN_EDGE_RCT2_COUNT)
        edgeStyle = TERRAIN_EDGE_ROCK;

    uint32_t base_image_id = get_edge_image(edgeStyle, 0);
    if (session->ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
    {
        base_image_id = get_edge_image(edgeStyle, 1);
    }

    if (edge == EDGE_BOTTOMRIGHT)
    {
        base_image_id += 5;
    }

    uint8_t curHeight = std::min(neighbourCornerHeight1, neighbourCornerHeight2);
    if (neighbourCornerHeight2 != neighbourCornerHeight1)
    {
        // If bottom part of edge isn't straight, add a filler
        uint32_t image_offset = 3;

        if (neighbourCornerHeight2 >= neighbourCornerHeight1)
        {
            image_offset = 4;
        }

        if (curHeight != cornerHeight1 && curHeight != cornerHeight2)
        {
            uint32_t image_id = base_image_id + image_offset;
            sub_98196C(session, image_id, offset.x, offset.y, bounds.x, bounds.y, 15, curHeight * 16);
            curHeight++;
        }
    }

    neighbourCornerHeight1 = cornerHeight2;

    for (uint32_t tunnelIndex = 0; tunnelIndex < TUNNEL_MAX_COUNT;)
    {
        if (curHeight >= cornerHeight1 || curHeight >= cornerHeight2)
        {
            // If top of edge isn't straight, add a filler
            uint32_t image_offset = 1;
            if (curHeight >= cornerHeight1)
            {
                image_offset = 2;

                if (curHeight >= cornerHeight2)
                {
                    return;
                }
            }

            const uint32_t image_id = base_image_id + image_offset;
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

            if (isWater || curHeight != tunnelArray[tunnelIndex].height)
            {
                sub_98196C(session, base_image_id, offset.x, offset.y, bounds.x, bounds.y, 15, curHeight * 16);

                curHeight++;
                continue;
            }
        }

        // Tunnels
        uint8_t tunnelType = tunnelArray[tunnelIndex].type;
        uint8_t tunnelHeight = _tunnelHeights[tunnelType][0];
        int16_t zOffset = curHeight;

        if ((zOffset + tunnelHeight) > neighbourCornerHeight1 || (zOffset + tunnelHeight) > cornerHeight1)
        {
            tunnelType = byte_97B5B0[tunnelType];
        }

        zOffset *= 16;

        int16_t boundBoxOffsetZ = zOffset + _boundBoxZOffsets[tunnelType];
        int8_t boundBoxLength = _tunnelHeights[tunnelType][1] * 16;
        if (boundBoxOffsetZ < 16)
        {
            boundBoxOffsetZ += 16;
            boundBoxLength -= 16;
        }

        uint32_t image_id = get_tunnel_image(edgeStyle, tunnelType) + (edge == EDGE_BOTTOMRIGHT ? 2 : 0);
        sub_98197C(
            session, image_id, offset.x, offset.y, tunnelBounds.x, tunnelBounds.y, boundBoxLength - 1, zOffset, 0, 0,
            boundBoxOffsetZ);

        boundBoxOffsetZ = curHeight * 16;
        boundBoxLength = _tunnelHeights[tunnelType][1] * 16;
        boundBoxOffsetZ += _boundBoxZOffsets[tunnelType];
        if (boundBoxOffsetZ == 0)
        {
            boundBoxOffsetZ += 16;
            boundBoxLength -= 16;
        }

        image_id = get_tunnel_image(edgeStyle, tunnelType) + (edge == EDGE_BOTTOMRIGHT ? 2 : 0) + 1;
        sub_98197C(
            session, image_id, offset.x, offset.y, tunnelBounds.x, tunnelBounds.y, boundBoxLength - 1, curHeight * 16,
            tunnelTopBoundBoxOffset.x, tunnelTopBoundBoxOffset.y, boundBoxOffsetZ);

        curHeight += _tunnelHeights[tunnelType][0];
        tunnelIndex++;
    }
}

/**
 *  rct2: 0x0065EB7D, 0x0065F0D8
 */
static void viewport_surface_draw_land_side_bottom(
    paint_session* session, enum edge_t edge, uint8_t height, uint8_t edgeStyle, struct tile_descriptor self,
    struct tile_descriptor neighbour)
{
    viewport_surface_draw_tile_side_bottom(session, edge, height, edgeStyle, self, neighbour, false);
}

/**
 *  rct2: 0x0065F8B9, 0x0065FE26
 */
static void viewport_surface_draw_water_side_bottom(
    paint_session* session, enum edge_t edge, uint8_t height, uint8_t edgeStyle, struct tile_descriptor self,
    struct tile_descriptor neighbour)
{
    viewport_surface_draw_tile_side_bottom(session, edge, height, edgeStyle, self, neighbour, true);
}

static void viewport_surface_draw_tile_side_top(
    paint_session* session, enum edge_t edge, uint8_t height, uint8_t terrain, struct tile_descriptor self,
    struct tile_descriptor neighbour, bool isWater)
{
    if (!is_csg_loaded() && terrain >= TERRAIN_EDGE_RCT2_COUNT)
        terrain = TERRAIN_EDGE_ROCK;

    int16_t al, ah, cl, ch, dl = 0, waterHeight;

    sLocationXY8 offset = { 0, 0 };
    sLocationXY8 bounds = { 0, 0 };

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

    if (isWater == false)
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
            waterHeight = neighbour.tile_element->AsSurface()->GetWaterHeight();
            if (dl == waterHeight)
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

    uint32_t base_image_id;

    if (isWater)
    {
        base_image_id = get_edge_image(terrain, 2); // var_08
        if (session->ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
        {
            base_image_id = get_edge_image(terrain, 1); // var_04
        }
        base_image_id += (edge == EDGE_TOPLEFT ? 5 : 0);
    }
    else
    {
        if (!(session->ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE))
        {
            const uint8_t incline = (cl - al) + 1;
            const uint32_t image_id = get_edge_image(terrain, 3) + (edge == EDGE_TOPLEFT ? 3 : 0) + incline; // var_c;
            const int16_t y = (dl - al) * 16;
            paint_attach_to_previous_ps(session, image_id, 0, y);
            return;
        }
        base_image_id = get_edge_image(terrain, 1) + (edge == EDGE_TOPLEFT ? 5 : 0); // var_04
    }

    uint8_t cur_height = std::min(ch, ah);
    if (ch != ah)
    {
        // neighbour tile corners aren't level
        uint32_t image_offset = 3;
        if (ch > ah)
        {
            image_offset = 4;
        }

        if (cur_height != al && cur_height != cl)
        {
            const uint32_t image_id = base_image_id + image_offset;
            sub_98196C(session, image_id, offset.x, offset.y, bounds.x, bounds.y, 15, cur_height * 16);
            cur_height++;
        }
    }

    ah = cl;

    if (isWater)
    {
        offset.x = 0;
        offset.y = 0;
    }

    while (cur_height < al && cur_height < ah)
    {
        sub_98196C(session, base_image_id, offset.x, offset.y, bounds.x, bounds.y, 15, cur_height * 16);
        cur_height++;
    }

    uint32_t image_offset = 1;
    if (cur_height >= al)
    {
        image_offset = 2;

        if (cur_height >= ah)
        {
            return;
        }
    }

    const uint32_t image_id = base_image_id + image_offset;
    sub_98196C(session, image_id, offset.x, offset.y, bounds.x, bounds.y, 15, cur_height * 16);
}

/**
 *  rct2: 0x0065F63B, 0x0065F77D
 */
static void viewport_surface_draw_land_side_top(
    paint_session* session, enum edge_t edge, uint8_t height, uint8_t terrain, struct tile_descriptor self,
    struct tile_descriptor neighbour)
{
    viewport_surface_draw_tile_side_top(session, edge, height, terrain, self, neighbour, false);
}

/**
 *  rct2: 0x0066039B, 0x006604F1
 */
static void viewport_surface_draw_water_side_top(
    paint_session* session, enum edge_t edge, uint8_t height, uint8_t terrain, struct tile_descriptor self,
    struct tile_descriptor neighbour)
{
    viewport_surface_draw_tile_side_top(session, edge, height, terrain, self, neighbour, true);
}

/**
 *  rct2: 0x0066062C
 */
void surface_paint(paint_session* session, uint8_t direction, uint16_t height, const TileElement* tileElement)
{
    rct_drawpixelinfo* dpi = &session->DPI;
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_TERRAIN;
    session->DidPassSurface = true;
    session->SurfaceElement = tileElement;

    const uint16_t zoomLevel = dpi->zoom_level;
    const uint8_t rotation = session->CurrentRotation;
    const uint32_t terrain_type = tileElement->AsSurface()->GetSurfaceStyle();
    const uint8_t surfaceShape = viewport_surface_paint_setup_get_relative_slope(tileElement, rotation);
    const LocationXY16& base = session->SpritePosition;
    const corner_height& cornerHeights = corner_heights[surfaceShape];

    tile_descriptor selfDescriptor = {
        { base.x / 32, base.y / 32 },
        tileElement,
        (uint8_t)terrain_type,
        surfaceShape,
        {
            (uint8_t)(height / 16 + cornerHeights.top),
            (uint8_t)(height / 16 + cornerHeights.right),
            (uint8_t)(height / 16 + cornerHeights.bottom),
            (uint8_t)(height / 16 + cornerHeights.left),
        },
    };

    tile_descriptor tileDescriptors[5];
    tileDescriptors[0] = selfDescriptor;

    for (int32_t i = 0; i < 4; i++)
    {
        const LocationXY16& offset = viewport_surface_paint_data[i][rotation];
        const CoordsXY position = {
            (int32_t)(base.x + offset.x),
            (int32_t)(base.y + offset.y),
        };

        tile_descriptor& descriptor = tileDescriptors[i + 1];

        descriptor.tile_element = nullptr;
        if (position.x > 0x2000 || position.y > 0x2000)
        {
            continue;
        }

        TileElement* surfaceElement = map_get_surface_element_at(position);
        if (surfaceElement == nullptr)
        {
            continue;
        }

        const uint32_t surfaceSlope = viewport_surface_paint_setup_get_relative_slope(surfaceElement, rotation);
        const uint8_t baseHeight = surfaceElement->base_height / 2;
        const corner_height& ch = corner_heights[surfaceSlope];

        descriptor.tile_coords = { position.x / 32, position.y / 32 };
        descriptor.tile_element = surfaceElement;
        descriptor.terrain = surfaceElement->AsSurface()->GetSurfaceStyle();
        descriptor.slope = surfaceSlope;
        descriptor.corner_heights.top = baseHeight + ch.top;
        descriptor.corner_heights.right = baseHeight + ch.right;
        descriptor.corner_heights.bottom = baseHeight + ch.bottom;
        descriptor.corner_heights.left = baseHeight + ch.left;
    }

    if ((session->ViewFlags & VIEWPORT_FLAG_LAND_HEIGHTS) && (zoomLevel == 0))
    {
        const int16_t x = session->MapPosition.x;
        const int16_t y = session->MapPosition.y;

        int32_t dx = tile_element_height(x + 16, y + 16) & 0xFFFF;
        dx += 3;

        int32_t image_id = (SPR_HEIGHT_MARKER_BASE + dx / 16) | 0x20780000;
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
        const bool showGridlines = (session->ViewFlags & VIEWPORT_FLAG_GRIDLINES);

        auto grassLength = -1;
        if (zoomLevel == 0)
        {
            if ((session->ViewFlags & (VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_UNDERGROUND_INSIDE)) == 0)
            {
                grassLength = tileElement->AsSurface()->GetGrassLength() & 0x7;
            }
        }

        assert(surfaceShape < std::size(byte_97B444));
        const uint8_t image_offset = byte_97B444[surfaceShape];

        auto imageId = get_surface_image(session, terrain_type, image_offset, rotation, grassLength, showGridlines, false);
        if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        {
            imageId = SPR_TERRAIN_TRACK_DESIGNER;
        }
        if (session->ViewFlags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE))
        {
            imageId &= 0xDC07FFFF; // remove colour
            imageId |= 0x41880000;
        }

        sub_98196C(session, imageId, 0, 0, 32, 32, -1, height);
        has_surface = true;
    }

    // Draw Staff Patrol Areas
    // loc_660D02
    if (gStaffDrawPatrolAreas != SPRITE_INDEX_NULL)
    {
        const int32_t staffIndex = gStaffDrawPatrolAreas;
        const bool is_staff_list = staffIndex & 0x8000;
        const int16_t x = session->MapPosition.x, y = session->MapPosition.y;

        uint8_t staffType = staffIndex & 0x7FFF;
        uint32_t image_id = IMAGE_TYPE_REMAP;
        uint8_t patrolColour = COLOUR_LIGHT_BLUE;

        if (!is_staff_list)
        {
            Peep* staff = GET_PEEP(staffIndex);
            if (!staff_is_patrol_area_set(staff->staff_id, x, y))
            {
                patrolColour = COLOUR_GREY;
            }
            staffType = staff->staff_type;
        }

        if (staff_is_patrol_area_set(200 + staffType, x, y))
        {
            assert(surfaceShape < std::size(byte_97B444));

            image_id |= SPR_TERRAIN_SELECTION_PATROL_AREA + byte_97B444[surfaceShape];
            image_id |= patrolColour << 19;
            paint_attach_to_previous_ps(session, image_id, 0, 0);
        }
    }

    // Draw Peep Spawns
    if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode)
        && session->ViewFlags & VIEWPORT_FLAG_LAND_OWNERSHIP)
    {
        const LocationXY16& pos = session->MapPosition;
        for (auto& spawn : gPeepSpawns)
        {
            if ((spawn.x & 0xFFE0) == pos.x && (spawn.y & 0xFFE0) == pos.y)
            {
                sub_98196C(session, SPR_TERRAIN_SELECTION_SQUARE_SIMPLE, 0, 0, 32, 32, 16, spawn.z);

                const int32_t offset = (direction_reverse(spawn.direction) + rotation) & 3;
                const uint32_t image_id = (PEEP_SPAWN_ARROW_0 + offset) | 0x20380000;
                sub_98196C(session, image_id, 0, 0, 32, 32, 19, spawn.z);
            }
        }
    }

    if (session->ViewFlags & VIEWPORT_FLAG_LAND_OWNERSHIP)
    {
        // loc_660E9A:
        if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_OWNED)
        {
            assert(surfaceShape < std::size(byte_97B444));
            paint_attach_to_previous_ps(session, SPR_TERRAIN_SELECTION_SQUARE + byte_97B444[surfaceShape], 0, 0);
        }
        else if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_AVAILABLE)
        {
            const LocationXY16& pos = session->MapPosition;
            const int32_t height2 = (tile_element_height(pos.x + 16, pos.y + 16) & 0xFFFF) + 3;
            paint_struct* backup = session->LastRootPS;
            sub_98196C(session, SPR_LAND_OWNERSHIP_AVAILABLE, 16, 16, 1, 1, 0, height2);
            session->LastRootPS = backup;
        }
    }

    if (session->ViewFlags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS && !(tileElement->AsSurface()->GetOwnership() & OWNERSHIP_OWNED))
    {
        if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
        {
            assert(surfaceShape < std::size(byte_97B444));
            paint_attach_to_previous_ps(session, SPR_TERRAIN_SELECTION_DOTTED + byte_97B444[surfaceShape], 0, 0);
        }
        else if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE)
        {
            const LocationXY16& pos = session->MapPosition;
            const int32_t height2 = tile_element_height(pos.x + 16, pos.y + 16) & 0xFFFF;
            paint_struct* backup = session->LastRootPS;
            sub_98196C(session, SPR_LAND_CONSTRUCTION_RIGHTS_AVAILABLE, 16, 16, 1, 1, 0, height2 + 3);
            session->LastRootPS = backup;
        }
    }

    // ebx[0] = esi;
    // ebp[4] = ebp;
    // ebp[8] = ebx

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
    {
        // loc_660FB8:
        const LocationXY16& pos = session->MapPosition;
        if (pos.x >= gMapSelectPositionA.x && pos.x <= gMapSelectPositionB.x && pos.y >= gMapSelectPositionA.y
            && pos.y <= gMapSelectPositionB.y)
        {
            const uint16_t mapSelectionType = gMapSelectType;
            if (mapSelectionType >= MAP_SELECT_TYPE_EDGE_0)
            {
                // Walls
                // loc_661089:
                const uint32_t eax = ((((mapSelectionType - 9) + rotation) & 3) + 0x21) << 19;
                const uint32_t image_id = (SPR_TERRAIN_SELECTION_EDGE + byte_97B444[surfaceShape]) | eax | IMAGE_TYPE_REMAP;
                paint_attach_to_previous_ps(session, image_id, 0, 0);
            }
            else if (mapSelectionType >= MAP_SELECT_TYPE_QUARTER_0)
            {
                // loc_661051:(no jump)
                // Selection split into four quarter segments
                const uint32_t eax = ((((mapSelectionType - MAP_SELECT_TYPE_QUARTER_0) + rotation) & 3) + 0x27) << 19;
                const uint32_t image_id = (SPR_TERRAIN_SELECTION_QUARTER + byte_97B444[surfaceShape]) | eax | IMAGE_TYPE_REMAP;
                paint_attach_to_previous_ps(session, image_id, 0, 0);
            }
            else if (mapSelectionType <= MAP_SELECT_TYPE_FULL)
            {
                // Corners
                uint32_t eax = mapSelectionType;
                if (mapSelectionType != MAP_SELECT_TYPE_FULL)
                {
                    eax = (mapSelectionType + rotation) & 3;
                }

                eax = (eax + 0x21) << 19;
                const uint32_t image_id = (SPR_TERRAIN_SELECTION_CORNER + byte_97B444[surfaceShape]) | eax | IMAGE_TYPE_REMAP;
                paint_attach_to_previous_ps(session, image_id, 0, 0);
            }
            else
            {
                int32_t local_surfaceShape = surfaceShape;
                int32_t local_height = height;
                // Water tool
                if (tileElement->AsSurface()->GetWaterHeight() > 0)
                {
                    int32_t waterHeight = tileElement->AsSurface()->GetWaterHeight() * 16;
                    if (waterHeight > height)
                    {
                        local_height += 16;

                        if (waterHeight != local_height || !(local_surfaceShape & 0x10))
                        {
                            local_height = waterHeight;
                            local_surfaceShape = 0;
                        }
                        else
                        {
                            int16_t bl, bh;

                            bl = (surfaceShape ^ 0xF) << 2;
                            bh = bl >> 4;
                            local_surfaceShape = (bh & 0x3) | (bl & 0xC);
                        }
                    }
                }

                const int32_t image_id = (SPR_TERRAIN_SELECTION_CORNER + byte_97B444[local_surfaceShape]) | 0x21300000;

                paint_struct* backup = session->LastRootPS;
                sub_98196C(session, image_id, 0, 0, 32, 32, 1, local_height);
                session->LastRootPS = backup;
            }
        }
    }

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        const LocationXY16& pos = session->MapPosition;

        for (const auto& tile : gMapSelectionTiles)
        {
            if (tile.x != pos.x || tile.y != pos.y)
            {
                continue;
            }

            uint32_t colours = COLOUR_GREY << 24 | COLOUR_BRIGHT_PURPLE << 19;
            if (gMapSelectFlags & MAP_SELECT_FLAG_GREEN)
            {
                colours = COLOUR_GREY << 24 | COLOUR_SATURATED_GREEN << 19;
            }

            const uint32_t image_id = (SPR_TERRAIN_SELECTION_CORNER + byte_97B444[surfaceShape]) | colours | IMAGE_TYPE_REMAP;
            paint_attach_to_previous_ps(session, image_id, 0, 0);
            break;
        }
    }

    if (zoomLevel == 0 && has_surface && !(session->ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
        && !(session->ViewFlags & VIEWPORT_FLAG_HIDE_BASE) && gConfigGeneral.landscape_smoothing)
    {
        viewport_surface_smoothen_edge(session, EDGE_TOPLEFT, tileDescriptors[0], tileDescriptors[3]);
        viewport_surface_smoothen_edge(session, EDGE_TOPRIGHT, tileDescriptors[0], tileDescriptors[4]);
        viewport_surface_smoothen_edge(session, EDGE_BOTTOMLEFT, tileDescriptors[0], tileDescriptors[1]);
        viewport_surface_smoothen_edge(session, EDGE_BOTTOMRIGHT, tileDescriptors[0], tileDescriptors[2]);
    }

    if ((session->ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE) && !(session->ViewFlags & VIEWPORT_FLAG_HIDE_BASE)
        && !(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        const uint8_t image_offset = byte_97B444[surfaceShape];
        const uint32_t image_id = get_surface_image(session, terrain_type, image_offset, rotation, 1, false, true);
        paint_attach_to_previous_ps(session, image_id, 0, 0);
    }

    if (!(session->ViewFlags & VIEWPORT_FLAG_HIDE_VERTICAL))
    {
        const uint32_t edgeStyle = tileElement->AsSurface()->GetEdgeStyle();
        if (edgeStyle >= TERRAIN_EDGE_COUNT)
        {
            log_verbose("edgeStyle: %d", edgeStyle);
        }

        tunnel_entry backupLeftTunnels[TUNNEL_MAX_COUNT];
        tunnel_entry backupRightTunnels[TUNNEL_MAX_COUNT];

        std::memcpy(backupLeftTunnels, session->LeftTunnels, sizeof(tunnel_entry) * TUNNEL_MAX_COUNT);
        std::memcpy(backupRightTunnels, session->RightTunnels, sizeof(tunnel_entry) * TUNNEL_MAX_COUNT);

        viewport_surface_draw_land_side_top(
            session, EDGE_TOPLEFT, height / 16, edgeStyle, tileDescriptors[0], tileDescriptors[3]);
        viewport_surface_draw_land_side_top(
            session, EDGE_TOPRIGHT, height / 16, edgeStyle, tileDescriptors[0], tileDescriptors[4]);
        viewport_surface_draw_land_side_bottom(
            session, EDGE_BOTTOMLEFT, height / 16, edgeStyle, tileDescriptors[0], tileDescriptors[1]);
        viewport_surface_draw_land_side_bottom(
            session, EDGE_BOTTOMRIGHT, height / 16, edgeStyle, tileDescriptors[0], tileDescriptors[2]);

        std::memcpy(session->LeftTunnels, backupLeftTunnels, sizeof(tunnel_entry) * TUNNEL_MAX_COUNT);
        std::memcpy(session->RightTunnels, backupRightTunnels, sizeof(tunnel_entry) * TUNNEL_MAX_COUNT);
    }

    if (tileElement->AsSurface()->GetWaterHeight() > 0)
    {
        // loc_6615A9: (water height)
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_WATER;

        const uint16_t localHeight = height + 16;
        const uint16_t waterHeight = tileElement->AsSurface()->GetWaterHeight() * 16;

        if (!gTrackDesignSaveMode)
        {
            session->WaterHeight = waterHeight;

            int32_t image_offset = 0;
            if (waterHeight <= localHeight)
            {
                image_offset = byte_97B740[surfaceShape & 0xF];
            }

            const int32_t image_id = (SPR_WATER_MASK + image_offset) | IMAGE_TYPE_REMAP | IMAGE_TYPE_TRANSPARENT
                | PALETTE_WATER << 19;
            sub_98196C(session, image_id, 0, 0, 32, 32, -1, waterHeight);

            paint_attach_to_previous_ps(session, SPR_WATER_OVERLAY + image_offset, 0, 0);

            // This wasn't in the original, but the code depended on globals that were only set in a different conditional
            const uint32_t edgeStyle = tileElement->AsSurface()->GetEdgeStyle();
            // end new code

            viewport_surface_draw_water_side_top(
                session, EDGE_TOPLEFT, waterHeight / 16, edgeStyle, tileDescriptors[0], tileDescriptors[3]);
            viewport_surface_draw_water_side_top(
                session, EDGE_TOPRIGHT, waterHeight / 16, edgeStyle, tileDescriptors[0], tileDescriptors[4]);
            viewport_surface_draw_water_side_bottom(
                session, EDGE_BOTTOMLEFT, waterHeight / 16, edgeStyle, tileDescriptors[0], tileDescriptors[1]);
            viewport_surface_draw_water_side_bottom(
                session, EDGE_BOTTOMRIGHT, waterHeight / 16, edgeStyle, tileDescriptors[0], tileDescriptors[2]);
        }
    }

    if ((tileElement->AsSurface()->GetParkFences()) && !gTrackDesignSaveMode)
    {
        // Owned land boundary fences
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_PARK;

        registers regs = {};
        regs.al = tileElement->AsSurface()->GetParkFences();
        regs.ax = regs.ax << rotation;
        regs.ah = regs.al >> 4;

        uint8_t al = regs.al | regs.ah;

        for (const auto& fenceData : _tileSurfaceBoundaries)
        {
            const int32_t bit = al & 1;
            al >>= 1;

            if (bit == 0)
                continue;

            int32_t local_height = height;
            int32_t image_id = 0;

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
            paint_util_set_segment_support_height(
                session,
                SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0
                    | SEGMENT_D4,
                height, 0);
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
