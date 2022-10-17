/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
#include "../../core/Numerics.hpp"
#include "../../drawing/Drawing.h"
#include "../../entity/EntityRegistry.h"
#include "../../entity/PatrolArea.h"
#include "../../entity/Peep.h"
#include "../../entity/Staff.h"
#include "../../interface/Colour.h"
#include "../../interface/Viewport.h"
#include "../../object/ObjectManager.h"
#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../paint/Paint.h"
#include "../../profiling/Profiling.h"
#include "../../ride/TrackDesign.h"
#include "../../sprites.h"
#include "../../world/Surface.h"
#include "../../world/TileInspector.h"
#include "../Boundbox.h"
#include "Paint.TileElement.h"

#include <algorithm>
#include <cstring>
#include <iterator>

static constexpr const uint8_t byte_97B444[] = {
    0, 2, 1, 3, 8, 10, 9, 11, 4, 6, 5, 7, 12, 14, 13, 15, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 16, 0, 18, 15, 0,
};

// rct2: 0x97B464, 0x97B474, 0x97B484, 0x97B494
static constexpr const CoordsXY viewport_surface_paint_data[][4] = {
    {
        { 32, 0 },
        { -32, 32 },
        { -64, -32 },
        { 0, -64 },
    },
    {
        { 0, 32 },
        { -64, 0 },
        { -32, -64 },
        { 32, -32 },
    },
    {
        { 0, -32 },
        { 0, 0 },
        { -32, 0 },
        { -32, -32 },
    },
    {
        { -32, 0 },
        { -32, -32 },
        { 0, -32 },
        { 0, 0 },
    },
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

// clang-format off
/**
*  rct2: 0x0097B4A4 (R), 0x0097B4C4 (T), 0x0097B4E4 (L), 0x0097B504 (B)
*/
static constexpr const corner_height corner_heights[] = {
    // T  R  B  L
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
    1, 2, 1, 3, 0, 3, 1, 5, 0,
};

// top left tint
static constexpr const uint32_t byte_97B537[] = {
    2, 5, 2, 4, 2, 5, 1, 1, 3, 4,
    3, 2, 1, 2, 0, 3, 1, 5, 0,
};

// top right tint
static constexpr const uint8_t byte_97B54A[] = {
    2, 2, 2, 4, 0, 0, 1, 1, 3, 4,
    3, 5, 1, 2, 2, 3, 1, 5, 0,
};

// bottom right tint
static constexpr const uint8_t byte_97B55D[] = {
    2, 2, 1, 4, 0, 0, 1, 2, 2, 4,
    1, 5, 1, 3, 2, 3, 1, 5, 0,
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

// clang-format on
// tunnel offset
static constexpr const uint8_t byte_97B5B0[TUNNEL_TYPE_COUNT] = {
    0, 0, 0, 3, 3, 3, 6, 6, 6, 6, 10, 11, 12, 13, 14, 14, 16, 17, 18, 19, 20, 21, 22,
};

static constexpr const uint8_t byte_97B740[] = {
    0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 1, 4, 0,
};

struct tile_descriptor
{
    TileCoordsXY tile_coords;
    const TileElement* tile_element;
    uint8_t terrain;
    uint8_t slope;
    corner_height corner_heights;
};

struct tile_surface_boundary_data
{
    int32_t bit_1;
    int32_t bit_8;
    int32_t bit_4;
    int32_t bit_2;
    uint32_t image[5];
    CoordsXY offset;
    BoundBoxXY Boundbox;
};

static constexpr const tile_surface_boundary_data _tileSurfaceBoundaries[4] = {
    {
        // Bottom right
        1,
        8,
        4,
        2,
        {
            SPR_TERRAIN_BOUNDARY_FENCES_1,
            SPR_TERRAIN_BOUNDARY_FENCES_5,
            SPR_TERRAIN_BOUNDARY_FENCES_3,
            SPR_TERRAIN_BOUNDARY_FENCES_3,
            SPR_TERRAIN_BOUNDARY_FENCES_5,
        },
        { 1, 31 },
        { { 1, 31 }, { 30, 1 } },
    },
    {
        // Bottom left
        1,
        2,
        4,
        8,
        {
            SPR_TERRAIN_BOUNDARY_FENCES_2,
            SPR_TERRAIN_BOUNDARY_FENCES_6,
            SPR_TERRAIN_BOUNDARY_FENCES_4,
            SPR_TERRAIN_BOUNDARY_FENCES_4,
            SPR_TERRAIN_BOUNDARY_FENCES_6,
        },
        { 31, 0 },
        { { 31, 1 }, { 1, 30 } },
    },
    {
        // Top left
        4,
        2,
        8,
        1,
        {
            SPR_TERRAIN_BOUNDARY_FENCES_1,
            SPR_TERRAIN_BOUNDARY_FENCES_3,
            SPR_TERRAIN_BOUNDARY_FENCES_5,
            SPR_TERRAIN_BOUNDARY_FENCES_3,
            SPR_TERRAIN_BOUNDARY_FENCES_5,
        },
        { 1, 0 },
        { { 1, 1 }, { 30, 1 } },
    },
    {
        // Top right
        4,
        8,
        2,
        1,
        {
            SPR_TERRAIN_BOUNDARY_FENCES_2,
            SPR_TERRAIN_BOUNDARY_FENCES_4,
            SPR_TERRAIN_BOUNDARY_FENCES_6,
            SPR_TERRAIN_BOUNDARY_FENCES_4,
            SPR_TERRAIN_BOUNDARY_FENCES_6,
        },
        { 1, 1 },
        { { 1, 1 }, { 1, 30 } },
    },
};

static const TerrainSurfaceObject* GetSurfaceObject(size_t index)
{
    PROFILED_FUNCTION();

    TerrainSurfaceObject* result{};
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::TerrainSurface, index);
    if (obj != nullptr)
    {
        return static_cast<TerrainSurfaceObject*>(obj);
    }
    return result;
}

static ImageId GetSurfaceImage(
    const PaintSession& session, ObjectEntryIndex index, int32_t offset, uint8_t rotation, int32_t grassLength, bool grid,
    bool underground)
{
    PROFILED_FUNCTION();

    ImageId image;
    auto obj = GetSurfaceObject(index);
    if (obj != nullptr)
    {
        image = ImageId(obj->GetImageId(
            { session.MapPosition.x >> 5, session.MapPosition.y >> 5 }, grassLength, rotation, offset, grid, underground));
        if (obj->Colour != 255)
        {
            image = image.WithPrimary(obj->Colour);
        }
    }
    return image;
}

static ImageId GetSurfacePattern(uint8_t index, int32_t offset)
{
    PROFILED_FUNCTION();

    ImageId image;
    auto obj = GetSurfaceObject(index);
    if (obj != nullptr)
    {
        image = ImageId(obj->PatternBaseImageId + offset);
        if (obj->Colour != 255)
        {
            image = image.WithPrimary(obj->Colour);
        }
    }
    return image;
}

static bool SurfaceShouldSmoothSelf(uint8_t index)
{
    PROFILED_FUNCTION();

    auto obj = GetSurfaceObject(index);
    if (obj != nullptr)
    {
        return obj->Flags & TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_SELF;
    }
    return false;
}

static bool SurfaceShouldSmooth(uint8_t index)
{
    PROFILED_FUNCTION();

    auto obj = GetSurfaceObject(index);
    if (obj != nullptr)
    {
        return obj->Flags & TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_OTHER;
    }
    return false;
}

static ImageId GetEdgeImageWithOffset(uint8_t index, uint32_t offset)
{
    ImageId result;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::TerrainEdge, index);
    if (obj != nullptr)
    {
        auto tobj = static_cast<TerrainEdgeObject*>(obj);
        result = ImageId(tobj->BaseImageId + offset);
    }
    return result;
}

static ImageId GetEdgeImage(uint8_t index, uint8_t type)
{
    static constexpr uint32_t offsets[] = {
        0,
        20,
        10,
        30,
    };

    ImageId result;
    if (type < std::size(offsets))
    {
        result = GetEdgeImageWithOffset(index, offsets[type]);
    }
    return result;
}

static ImageId GetTunnelImage(ObjectEntryIndex index, uint8_t type, edge_t edge)
{
    PROFILED_FUNCTION();

    static constexpr uint32_t offsets[TUNNEL_TYPE_COUNT] = { 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80,
                                                             36, 48, 60, 72, 76, 80, 84, 88, 92, 96, 100 };

    bool hasDoors = false;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::TerrainEdge, index);
    if (obj != nullptr)
    {
        auto tobj = static_cast<TerrainEdgeObject*>(obj);
        hasDoors = tobj->HasDoors && !tobj->UsesFallbackImages();
    }

    if (!hasDoors && type >= REGULAR_TUNNEL_TYPE_COUNT && type < std::size(offsets))
        type = TUNNEL_0;

    ImageId result;
    if (type < std::size(offsets))
    {
        result = GetEdgeImageWithOffset(index, offsets[type]).WithIndexOffset(edge == EDGE_BOTTOMRIGHT ? 2 : 0);
    }
    return result;
}

static uint8_t ViewportSurfacePaintSetupGetRelativeSlope(const SurfaceElement& surfaceElement, int32_t rotation)
{
    const uint8_t slope = surfaceElement.GetSlope();
    const uint8_t slopeHeight = slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
    uint16_t slopeCorners = (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) << rotation;
    slopeCorners = ((slopeCorners >> 4) | slopeCorners) & 0x0F;
    return slopeHeight | slopeCorners;
}

/**
 *  rct2: 0x0065E890, 0x0065E946, 0x0065E9FC, 0x0065EAB2
 */
static void ViewportSurfaceSmoothenEdge(
    PaintSession& session, enum edge_t edge, struct tile_descriptor self, struct tile_descriptor neighbour)
{
    PROFILED_FUNCTION();

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

        if (!SurfaceShouldSmoothSelf(self.terrain))
            return;
    }
    else
    {
        if (!SurfaceShouldSmooth(self.terrain))
            return;

        if (!SurfaceShouldSmooth(neighbour.terrain))
            return;
    }

    const auto image_id = ImageId(maskImageBase + byte_97B444[self.slope]);

    if (PaintAttachToPreviousPS(session, image_id, 0, 0))
    {
        AttachedPaintStruct* out = session.LastAttachedPS;
        // set content and enable masking
        out->ColourImageId = GetSurfacePattern(neighbour.terrain, cl);
        out->IsMasked = true;
    }
}

static bool TileIsInsideClipView(const tile_descriptor& tile)
{
    if (tile.tile_element == nullptr)
        return false;

    if (tile.tile_element->GetBaseZ() > gClipHeight * COORDS_Z_STEP)
        return false;

    auto coords = tile.tile_coords.ToCoordsXY();
    if (coords.x < gClipSelectionA.x || coords.x > gClipSelectionB.x)
        return false;
    if (coords.y < gClipSelectionA.y || coords.y > gClipSelectionB.y)
        return false;

    return true;
}

static void ViewportSurfaceDrawTileSideBottom(
    PaintSession& session, enum edge_t edge, uint16_t height, uint8_t edgeStyle, struct tile_descriptor self,
    struct tile_descriptor neighbour, bool isWater)
{
    PROFILED_FUNCTION();

    // From big Z to tiny Z
    height /= COORDS_Z_PER_TINY_Z;
    int16_t cornerHeight1, neighbourCornerHeight1, cornerHeight2, neighbourCornerHeight2;

    CoordsXY offset = { 0, 0 };
    CoordsXY bounds = { 0, 0 };
    CoordsXY tunnelBounds = { 1, 1 };
    CoordsXY tunnelTopBoundBoxOffset = { 0, 0 };

    const TunnelEntry* tunnelArray;
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

            tunnelArray = session.LeftTunnels;
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

            tunnelArray = session.RightTunnels;
            break;

        default:
            return;
    }

    bool neighbourIsClippedAway = (session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW) && !TileIsInsideClipView(neighbour);

    if (neighbour.tile_element == nullptr || neighbourIsClippedAway)
    {
        // The neighbour tile doesn't exist or isn't drawn - assume minimum height to draw full edges
        neighbourCornerHeight2 = MINIMUM_LAND_HEIGHT / 2;
        neighbourCornerHeight1 = MINIMUM_LAND_HEIGHT / 2;
    }

    if (isWater && neighbour.tile_element != nullptr)
    {
        auto waterHeight = neighbour.tile_element->AsSurface()->GetWaterHeight() / (COORDS_Z_STEP * 2);
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

    auto baseImageId = GetEdgeImage(edgeStyle, 0);
    if (session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
    {
        baseImageId = GetEdgeImage(edgeStyle, 1);
    }

    if (edge == EDGE_BOTTOMRIGHT)
    {
        baseImageId = baseImageId.WithIndexOffset(5);
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
            auto imageId = baseImageId.WithIndexOffset(image_offset);
            PaintAddImageAsParent(session, imageId, { offset, curHeight * COORDS_Z_PER_TINY_Z }, { bounds, 15 });
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

            auto imageId = baseImageId.WithIndexOffset(image_offset);
            PaintAddImageAsParent(session, imageId, { offset, curHeight * COORDS_Z_PER_TINY_Z }, { bounds, 15 });

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
                PaintAddImageAsParent(session, baseImageId, { offset, curHeight * COORDS_Z_PER_TINY_Z }, { bounds, 15 });

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

        auto imageId = GetTunnelImage(edgeStyle, tunnelType, edge);
        PaintAddImageAsParent(
            session, imageId, { offset, zOffset }, { tunnelBounds.x, tunnelBounds.y, boundBoxLength - 1 },
            { 0, 0, boundBoxOffsetZ });

        boundBoxOffsetZ = curHeight * COORDS_Z_PER_TINY_Z;
        boundBoxLength = _tunnelHeights[tunnelType][1] * 16;
        boundBoxOffsetZ += _boundBoxZOffsets[tunnelType];
        if (boundBoxOffsetZ == 0)
        {
            boundBoxOffsetZ += 16;
            boundBoxLength -= 16;
        }

        imageId = GetTunnelImage(edgeStyle, tunnelType, edge).WithIndexOffset(1);
        PaintAddImageAsParent(
            session, imageId, { offset, curHeight * COORDS_Z_PER_TINY_Z },
            { tunnelBounds.x, tunnelBounds.y, boundBoxLength - 1 },
            { tunnelTopBoundBoxOffset.x, tunnelTopBoundBoxOffset.y, boundBoxOffsetZ });

        curHeight += _tunnelHeights[tunnelType][0];
        tunnelIndex++;
    }
}

/**
 *  rct2: 0x0065EB7D, 0x0065F0D8
 */
static void ViewportSurfaceDrawLandSideBottom(
    PaintSession& session, enum edge_t edge, uint16_t height, uint8_t edgeStyle, struct tile_descriptor self,
    struct tile_descriptor neighbour)
{
    ViewportSurfaceDrawTileSideBottom(session, edge, height, edgeStyle, self, neighbour, false);
}

/**
 *  rct2: 0x0065F8B9, 0x0065FE26
 */
static void ViewportSurfaceDrawWaterSideBottom(
    PaintSession& session, enum edge_t edge, uint16_t height, uint8_t edgeStyle, struct tile_descriptor self,
    struct tile_descriptor neighbour)
{
    ViewportSurfaceDrawTileSideBottom(session, edge, height, edgeStyle, self, neighbour, true);
}

static void ViewportSurfaceDrawTileSideTop(
    PaintSession& session, enum edge_t edge, uint16_t height, uint8_t terrain, struct tile_descriptor self,
    struct tile_descriptor neighbour, bool isWater)
{
    PROFILED_FUNCTION();

    // From big Z to tiny Z
    height /= COORDS_Z_PER_TINY_Z;

    int16_t cornerHeight1, neighbourCornerHeight1, cornerHeight2, neighbourCornerHeight2;

    CoordsXY offset = { 0, 0 };
    CoordsXY bounds = { 0, 0 };

    switch (edge)
    {
        case EDGE_TOPLEFT:
            cornerHeight1 = self.corner_heights.top;
            cornerHeight2 = self.corner_heights.left;

            neighbourCornerHeight1 = neighbour.corner_heights.right;
            neighbourCornerHeight2 = neighbour.corner_heights.bottom;

            offset.y = -2;
            bounds.x = 30;
            break;

        case EDGE_TOPRIGHT:
            cornerHeight1 = self.corner_heights.top;
            cornerHeight2 = self.corner_heights.right;

            neighbourCornerHeight1 = neighbour.corner_heights.left;
            neighbourCornerHeight2 = neighbour.corner_heights.bottom;

            offset.x = -2;
            bounds.y = 30;
            break;

        default:
            return;
    }

    // save ecx
    if (neighbour.tile_element == nullptr)
    {
        neighbourCornerHeight1 = 1;
        neighbourCornerHeight2 = 1;
    }
    else
    {
        if (isWater)
        {
            auto waterHeight = neighbour.tile_element->AsSurface()->GetWaterHeight() / (COORDS_Z_STEP * 2);
            if (height == waterHeight)
            {
                return;
            }

            cornerHeight1 = height;
            cornerHeight2 = height;
        }
    }

    if (cornerHeight1 <= neighbourCornerHeight1 && cornerHeight2 <= neighbourCornerHeight2)
    {
        return;
    }

    ImageId baseImageId;
    if (isWater)
    {
        baseImageId = GetEdgeImage(terrain, 2); // var_08
        if (session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
        {
            baseImageId = GetEdgeImage(terrain, 1); // var_04
        }
        baseImageId = baseImageId.WithIndexOffset(edge == EDGE_TOPLEFT ? 5 : 0);
    }
    else
    {
        if (!(session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE))
        {
            const uint8_t incline = (cornerHeight2 - cornerHeight1) + 1;
            const auto imageId = GetEdgeImage(terrain, 3).WithIndexOffset((edge == EDGE_TOPLEFT ? 3 : 0) + incline);
            const int16_t y = (height - cornerHeight1) * COORDS_Z_PER_TINY_Z;
            PaintAttachToPreviousPS(session, imageId, 0, y);
            return;
        }
        baseImageId = GetEdgeImage(terrain, 1).WithIndexOffset(edge == EDGE_TOPLEFT ? 5 : 0);
    }

    uint8_t cur_height = std::min(neighbourCornerHeight2, neighbourCornerHeight1);
    if (neighbourCornerHeight2 != neighbourCornerHeight1)
    {
        // neighbour tile corners aren't level
        uint32_t image_offset = 3;
        if (neighbourCornerHeight2 > neighbourCornerHeight1)
        {
            image_offset = 4;
        }

        if (cur_height != cornerHeight1 && cur_height != cornerHeight2)
        {
            auto imageId = baseImageId.WithIndexOffset(image_offset);
            PaintAddImageAsParent(
                session, imageId, { offset.x, offset.y, cur_height * COORDS_Z_PER_TINY_Z }, { bounds.x, bounds.y, 15 });
            cur_height++;
        }
    }

    neighbourCornerHeight1 = cornerHeight2;

    if (isWater)
    {
        offset.x = 0;
        offset.y = 0;
    }

    while (cur_height < cornerHeight1 && cur_height < neighbourCornerHeight1)
    {
        PaintAddImageAsParent(session, baseImageId, { offset, cur_height * COORDS_Z_PER_TINY_Z }, { bounds, 15 });
        cur_height++;
    }

    uint32_t image_offset = 1;
    if (cur_height >= cornerHeight1)
    {
        image_offset = 2;

        if (cur_height >= neighbourCornerHeight1)
        {
            return;
        }
    }

    auto imageId = baseImageId.WithIndexOffset(image_offset);
    PaintAddImageAsParent(session, imageId, { offset, cur_height * COORDS_Z_PER_TINY_Z }, { bounds, 15 });
}

/**
 *  rct2: 0x0065F63B, 0x0065F77D
 */
static void ViewportSurfaceDrawLandSideTop(
    PaintSession& session, enum edge_t edge, uint16_t height, uint8_t terrain, struct tile_descriptor self,
    struct tile_descriptor neighbour)
{
    ViewportSurfaceDrawTileSideTop(session, edge, height, terrain, self, neighbour, false);
}

/**
 *  rct2: 0x0066039B, 0x006604F1
 */
static void ViewportSurfaceDrawWaterSideTop(
    PaintSession& session, enum edge_t edge, uint16_t height, uint8_t terrain, struct tile_descriptor self,
    struct tile_descriptor neighbour)
{
    ViewportSurfaceDrawTileSideTop(session, edge, height, terrain, self, neighbour, true);
}

static std::pair<int32_t, int32_t> SurfaceGetHeightAboveWater(
    const SurfaceElement& surfaceElement, const int32_t height, const int32_t surfaceShape)
{
    int32_t localSurfaceShape = surfaceShape;
    int32_t localHeight = height;

    if (surfaceElement.GetWaterHeight() > 0)
    {
        int32_t waterHeight = surfaceElement.GetWaterHeight();
        if (waterHeight > height)
        {
            localHeight += LAND_HEIGHT_STEP;

            if (waterHeight != localHeight || !(localSurfaceShape & TILE_ELEMENT_SURFACE_DIAGONAL_FLAG))
            {
                localHeight = waterHeight;
                localSurfaceShape = TILE_ELEMENT_SLOPE_FLAT;
            }
            else
            {
                localSurfaceShape = Numerics::ror4(surfaceShape ^ TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK, 2);
            }
        }
    }

    return { localHeight, localSurfaceShape };
}

std::optional<colour_t> GetPatrolAreaTileColour(const CoordsXY& pos)
{
    auto patrolAreaToRender = GetPatrolAreaToRender();
    if (const auto* staffType = std::get_if<StaffType>(&patrolAreaToRender))
    {
        if (IsPatrolAreaSetForStaffType(*staffType, pos))
        {
            return COLOUR_GREY;
        }
    }
    else
    {
        auto& staffId = std::get<EntityId>(patrolAreaToRender);
        auto* staff = GetEntity<Staff>(staffId);
        if (staff != nullptr)
        {
            if (staff->IsPatrolAreaSet(pos))
            {
                return COLOUR_LIGHT_BLUE;
            }
            else if (IsPatrolAreaSetForStaffType(staff->AssignedStaffType, pos))
            {
                return COLOUR_GREY;
            }
        }
    }
    return {};
}

static void PaintPatrolArea(PaintSession& session, const SurfaceElement& element, int32_t height, uint8_t surfaceShape)
{
    auto colour = GetPatrolAreaTileColour(session.MapPosition);
    if (colour)
    {
        assert(surfaceShape < std::size(byte_97B444));

        auto [localZ, localSurfaceShape] = SurfaceGetHeightAboveWater(element, height, surfaceShape);
        auto imageId = ImageId(SPR_TERRAIN_SELECTION_PATROL_AREA + byte_97B444[localSurfaceShape], *colour);

        auto* backup = session.LastPS;
        PaintAddImageAsParent(session, imageId, { 0, 0, localZ }, { 32, 32, 1 });
        session.LastPS = backup;
    }
}

/**
 *  rct2: 0x0066062C
 */
void PaintSurface(PaintSession& session, uint8_t direction, uint16_t height, const SurfaceElement& tileElement)
{
    PROFILED_FUNCTION();

    rct_drawpixelinfo* dpi = &session.DPI;
    session.InteractionType = ViewportInteractionItem::Terrain;
    session.Flags |= PaintSessionFlags::PassedSurface;
    session.SurfaceElement = reinterpret_cast<const TileElement*>(&tileElement);

    const auto zoomLevel = dpi->zoom_level;
    const uint8_t rotation = session.CurrentRotation;
    const auto terrain_type = tileElement.GetSurfaceStyle();
    const uint8_t surfaceShape = ViewportSurfacePaintSetupGetRelativeSlope(tileElement, rotation);
    const CoordsXY& base = session.SpritePosition;
    const corner_height& cornerHeights = corner_heights[surfaceShape];
    const TileElement* elementPtr = &reinterpret_cast<const TileElement&>(tileElement);

    tile_descriptor selfDescriptor = {
        TileCoordsXY(base),
        elementPtr,
        static_cast<uint8_t>(terrain_type),
        surfaceShape,
        {
            static_cast<uint8_t>(height / 16 + cornerHeights.top),
            static_cast<uint8_t>(height / 16 + cornerHeights.right),
            static_cast<uint8_t>(height / 16 + cornerHeights.bottom),
            static_cast<uint8_t>(height / 16 + cornerHeights.left),
        },
    };

    tile_descriptor tileDescriptors[5];
    tileDescriptors[0] = selfDescriptor;

    for (std::size_t i = 0; i < std::size(viewport_surface_paint_data); i++)
    {
        const CoordsXY& offset = viewport_surface_paint_data[i][rotation];
        const CoordsXY position = base + offset;

        tile_descriptor& descriptor = tileDescriptors[i + 1];

        descriptor.tile_element = nullptr;
        if (!MapIsLocationValid(position))
        {
            continue;
        }

        auto surfaceElement = MapGetSurfaceElementAt(position);
        if (surfaceElement == nullptr)
        {
            continue;
        }

        const uint32_t surfaceSlope = ViewportSurfacePaintSetupGetRelativeSlope(*surfaceElement, rotation);
        const uint8_t baseHeight = surfaceElement->GetBaseZ() / 16;
        const corner_height& ch = corner_heights[surfaceSlope];

        descriptor.tile_coords = TileCoordsXY{ position };
        descriptor.tile_element = reinterpret_cast<TileElement*>(surfaceElement);
        descriptor.terrain = surfaceElement->GetSurfaceStyle();
        descriptor.slope = surfaceSlope;
        descriptor.corner_heights.top = baseHeight + ch.top;
        descriptor.corner_heights.right = baseHeight + ch.right;
        descriptor.corner_heights.bottom = baseHeight + ch.bottom;
        descriptor.corner_heights.left = baseHeight + ch.left;
    }

    if (PaintShouldShowHeightMarkers(session, VIEWPORT_FLAG_LAND_HEIGHTS))
    {
        const int16_t x = session.MapPosition.x;
        const int16_t y = session.MapPosition.y;

        int32_t surfaceHeight = TileElementHeight({ x + 16, y + 16 });
        int32_t dx = surfaceHeight + 3;

        int32_t image_id = (SPR_HEIGHT_MARKER_BASE + dx / 16);
        image_id += get_height_marker_offset();
        image_id -= gMapBaseZ;

        PaintAddImageAsParent(session, ImageId(image_id, COLOUR_OLIVE_GREEN), { 16, 16, surfaceHeight }, { 1, 1, 0 });
    }

    bool has_surface = false;
    if (session.VerticalTunnelHeight * COORDS_Z_PER_TINY_Z == height)
    {
        // Vertical tunnels
        PaintAddImageAsParent(session, ImageId(1575), { 0, 0, height }, { 1, 30, 39 }, { -2, 1, height - 40 });
        PaintAddImageAsParent(session, ImageId(1576), { 0, 0, height }, { 30, 1, 0 }, { 1, 31, height });
        PaintAddImageAsParent(session, ImageId(1577), { 0, 0, height }, { 1, 30, 0 }, { 31, 1, height });
        PaintAddImageAsParent(session, ImageId(1578), { 0, 0, height }, { 30, 1, 39 }, { 1, -2, height - 40 });
    }
    else
    {
        const bool showGridlines = (session.ViewFlags & VIEWPORT_FLAG_GRIDLINES);

        auto grassLength = -1;
        if (zoomLevel <= ZoomLevel{ 0 })
        {
            if ((session.ViewFlags & (VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_UNDERGROUND_INSIDE)) == 0)
            {
                grassLength = tileElement.GetGrassLength() & 0x7;
            }
        }

        assert(surfaceShape < std::size(byte_97B444));
        const uint8_t image_offset = byte_97B444[surfaceShape];

        ImageId imageId;
        if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        {
            imageId = ImageId(SPR_TERRAIN_TRACK_DESIGNER);
        }
        else
        {
            imageId = GetSurfaceImage(session, terrain_type, image_offset, rotation, grassLength, showGridlines, false);
        }
        if (session.ViewFlags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE))
        {
            imageId = imageId.WithTransparency(FilterPaletteID::PaletteDarken1);
        }

        if (OpenRCT2::TileInspector::IsElementSelected(elementPtr))
        {
            imageId = imageId.WithRemap(FilterPaletteID::Palette44);
        }

        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, -1 });
        has_surface = true;
    }

    PaintPatrolArea(session, tileElement, height, surfaceShape);

    // Draw Peep Spawns
    if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode)
        && session.ViewFlags & VIEWPORT_FLAG_LAND_OWNERSHIP)
    {
        const CoordsXY& pos = session.MapPosition;
        for (auto& spawn : gPeepSpawns)
        {
            if ((spawn.x & 0xFFE0) == pos.x && (spawn.y & 0xFFE0) == pos.y)
            {
                PaintAddImageAsParent(session, ImageId(SPR_TERRAIN_SELECTION_SQUARE_SIMPLE), { 0, 0, spawn.z }, { 32, 32, 16 });

                const int32_t offset = (DirectionReverse(spawn.direction) + rotation) & 3;
                const auto image_id = ImageId(PEEP_SPAWN_ARROW_0 + offset, COLOUR_LIGHT_BLUE);
                PaintAddImageAsParent(session, image_id, { 0, 0, spawn.z }, { 32, 32, 19 });
            }
        }
    }

    if (session.ViewFlags & VIEWPORT_FLAG_LAND_OWNERSHIP)
    {
        // loc_660E9A:
        if (tileElement.GetOwnership() & OWNERSHIP_OWNED)
        {
            assert(surfaceShape < std::size(byte_97B444));
            PaintAttachToPreviousPS(session, ImageId(SPR_TERRAIN_SELECTION_SQUARE + byte_97B444[surfaceShape]), 0, 0);
        }
        else if (tileElement.GetOwnership() & OWNERSHIP_AVAILABLE)
        {
            const CoordsXY& pos = session.MapPosition;
            const int32_t height2 = (TileElementHeight({ pos.x + 16, pos.y + 16 })) + 3;
            PaintStruct* backup = session.LastPS;
            PaintAddImageAsParent(session, ImageId(SPR_LAND_OWNERSHIP_AVAILABLE), { 16, 16, height2 }, { 1, 1, 0 });
            session.LastPS = backup;
        }
    }

    if (session.ViewFlags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS && !(tileElement.GetOwnership() & OWNERSHIP_OWNED))
    {
        if (tileElement.GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
        {
            assert(surfaceShape < std::size(byte_97B444));
            PaintAttachToPreviousPS(session, ImageId(SPR_TERRAIN_SELECTION_DOTTED + byte_97B444[surfaceShape]), 0, 0);
        }
        else if (tileElement.GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE)
        {
            const CoordsXY& pos = session.MapPosition;
            const int32_t height2 = TileElementHeight({ pos.x + 16, pos.y + 16 });
            PaintStruct* backup = session.LastPS;
            PaintAddImageAsParent(
                session, ImageId(SPR_LAND_CONSTRUCTION_RIGHTS_AVAILABLE), { 16, 16, height2 + 3 }, { 1, 1, 0 });
            session.LastPS = backup;
        }
    }

    // ebx[0] = esi;
    // ebp[4] = ebp;
    // ebp[8] = ebx

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
    {
        // loc_660FB8:
        const CoordsXY& pos = session.MapPosition;
        if (pos.x >= gMapSelectPositionA.x && pos.x <= gMapSelectPositionB.x && pos.y >= gMapSelectPositionA.y
            && pos.y <= gMapSelectPositionB.y)
        {
            const uint16_t mapSelectionType = gMapSelectType;
            if (mapSelectionType >= MAP_SELECT_TYPE_EDGE_0)
            {
                // Walls
                // loc_661089:
                const auto fpId = static_cast<FilterPaletteID>((((mapSelectionType - 9) + rotation) & 3) + 0x21);
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_EDGE + byte_97B444[surfaceShape], fpId);
                PaintAttachToPreviousPS(session, image_id, 0, 0);
            }
            else if (mapSelectionType >= MAP_SELECT_TYPE_QUARTER_0)
            {
                // loc_661051:(no jump)
                // Selection split into four quarter segments
                const auto fpId = static_cast<FilterPaletteID>(
                    (((mapSelectionType - MAP_SELECT_TYPE_QUARTER_0) + rotation) & 3) + 0x27);
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_QUARTER + byte_97B444[surfaceShape], fpId);
                PaintAttachToPreviousPS(session, image_id, 0, 0);
            }
            else if (mapSelectionType <= MAP_SELECT_TYPE_FULL)
            {
                // Corners
                uint32_t eax = mapSelectionType;
                if (mapSelectionType != MAP_SELECT_TYPE_FULL)
                {
                    eax = (mapSelectionType + rotation) & 3;
                }

                const auto fpId = static_cast<FilterPaletteID>(eax + 0x21);
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_CORNER + byte_97B444[surfaceShape], fpId);
                PaintAttachToPreviousPS(session, image_id, 0, 0);
            }
            else
            {
                // The water tool should draw its grid _on_ the water, rather than on the surface under water.
                auto [local_height, local_surfaceShape] = SurfaceGetHeightAboveWater(tileElement, height, surfaceShape);

                const auto fpId = static_cast<FilterPaletteID>(38);
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_CORNER + byte_97B444[local_surfaceShape], fpId);

                PaintStruct* backup = session.LastPS;
                PaintAddImageAsParent(session, image_id, { 0, 0, local_height }, { 32, 32, 1 });
                session.LastPS = backup;
            }
        }
    }

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        const CoordsXY& pos = session.MapPosition;

        for (const auto& tile : gMapSelectionTiles)
        {
            if (tile.x != pos.x || tile.y != pos.y)
            {
                continue;
            }

            FilterPaletteID fpId = static_cast<FilterPaletteID>(37);
            if (gMapSelectFlags & MAP_SELECT_FLAG_GREEN)
            {
                fpId = static_cast<FilterPaletteID>(43);
            }

            const auto image_id = ImageId(SPR_TERRAIN_SELECTION_CORNER + byte_97B444[surfaceShape], fpId);
            PaintAttachToPreviousPS(session, image_id, 0, 0);
            break;
        }
    }

    if (zoomLevel <= ZoomLevel{ 0 } && has_surface && !(session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
        && !(session.ViewFlags & VIEWPORT_FLAG_HIDE_BASE) && gConfigGeneral.landscape_smoothing)
    {
        ViewportSurfaceSmoothenEdge(session, EDGE_TOPLEFT, tileDescriptors[0], tileDescriptors[3]);
        ViewportSurfaceSmoothenEdge(session, EDGE_TOPRIGHT, tileDescriptors[0], tileDescriptors[4]);
        ViewportSurfaceSmoothenEdge(session, EDGE_BOTTOMLEFT, tileDescriptors[0], tileDescriptors[1]);
        ViewportSurfaceSmoothenEdge(session, EDGE_BOTTOMRIGHT, tileDescriptors[0], tileDescriptors[2]);
    }

    if ((session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE) && !(session.ViewFlags & VIEWPORT_FLAG_HIDE_BASE)
        && !(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        const uint8_t image_offset = byte_97B444[surfaceShape];
        auto imageId = GetSurfaceImage(session, terrain_type, image_offset, rotation, 1, false, true);
        PaintAttachToPreviousPS(session, imageId, 0, 0);
    }

    if (!(session.ViewFlags & VIEWPORT_FLAG_HIDE_VERTICAL))
    {
        const auto edgeStyle = tileElement.GetEdgeStyle();
        if (static_cast<int32_t>(edgeStyle) >= object_entry_group_counts[EnumValue(ObjectType::TerrainEdge)])
        {
            log_verbose("edgeStyle: %d", edgeStyle);
        }

        ViewportSurfaceDrawLandSideTop(session, EDGE_TOPLEFT, height, edgeStyle, tileDescriptors[0], tileDescriptors[3]);
        ViewportSurfaceDrawLandSideTop(session, EDGE_TOPRIGHT, height, edgeStyle, tileDescriptors[0], tileDescriptors[4]);
        ViewportSurfaceDrawLandSideBottom(session, EDGE_BOTTOMLEFT, height, edgeStyle, tileDescriptors[0], tileDescriptors[1]);
        ViewportSurfaceDrawLandSideBottom(session, EDGE_BOTTOMRIGHT, height, edgeStyle, tileDescriptors[0], tileDescriptors[2]);
    }

    const uint16_t waterHeight = tileElement.GetWaterHeight();
    const bool waterGetsClipped = (session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW) && (waterHeight > gClipHeight * COORDS_Z_STEP);

    if (waterHeight > 0 && !gTrackDesignSaveMode && !waterGetsClipped)
    {
        // loc_6615A9: (water height)
        session.InteractionType = ViewportInteractionItem::Water;

        const uint16_t localHeight = height + 16;

        session.WaterHeight = waterHeight;

        int32_t image_offset = 0;
        if (waterHeight <= localHeight)
        {
            image_offset = byte_97B740[surfaceShape & 0xF];
        }

        const auto image_id = ImageId(SPR_WATER_MASK + image_offset, FilterPaletteID::PaletteWater).WithBlended(true);
        PaintAddImageAsParent(session, image_id, { 0, 0, waterHeight }, { 32, 32, -1 });

        const bool transparent = gConfigGeneral.transparent_water || (session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE);
        const uint32_t overlayStart = transparent ? SPR_WATER_OVERLAY : SPR_RCT1_WATER_OVERLAY;
        PaintAttachToPreviousPS(session, ImageId(overlayStart + image_offset), 0, 0);

        if (!(session.ViewFlags & VIEWPORT_FLAG_HIDE_VERTICAL))
        {
            // This wasn't in the original, but the code depended on globals that were only set in a different conditional
            const uint32_t edgeStyle = tileElement.GetEdgeStyle();
            // end new code

            ViewportSurfaceDrawWaterSideBottom(
                session, EDGE_BOTTOMLEFT, waterHeight, edgeStyle, tileDescriptors[0], tileDescriptors[1]);
            ViewportSurfaceDrawWaterSideBottom(
                session, EDGE_BOTTOMRIGHT, waterHeight, edgeStyle, tileDescriptors[0], tileDescriptors[2]);
            ViewportSurfaceDrawWaterSideTop(
                session, EDGE_TOPLEFT, waterHeight, edgeStyle, tileDescriptors[0], tileDescriptors[3]);
            ViewportSurfaceDrawWaterSideTop(
                session, EDGE_TOPRIGHT, waterHeight, edgeStyle, tileDescriptors[0], tileDescriptors[4]);
        }
    }

    if ((tileElement.GetParkFences()) && !gTrackDesignSaveMode)
    {
        // Owned land boundary fences
        session.InteractionType = ViewportInteractionItem::ParkEntrance;

        uint8_t rotatedFences = Numerics::rol4(tileElement.GetParkFences(), rotation);

        for (const auto& fenceData : _tileSurfaceBoundaries)
        {
            const int32_t edgeHasFence = rotatedFences & 1;
            rotatedFences >>= 1;

            if (edgeHasFence == 0)
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

            PaintAddImageAsParent(
                session, ImageId(image_id), { fenceData.offset, local_height }, { fenceData.Boundbox.length, 9 },
                { fenceData.Boundbox.offset, local_height + 1 });
        }
    }

    session.InteractionType = ViewportInteractionItem::Terrain;

    switch (surfaceShape)
    {
        default:
            // loc_661C2C
            //     00
            //   00  00
            // 00  00  00
            //   00  00
            //     00
            PaintUtilSetSegmentSupportHeight(
                session,
                SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0
                    | SEGMENT_D4,
                height, 0);
            PaintUtilForceSetGeneralSupportHeight(session, height, 0);
            break;

        case 1:
            // loc_661CB9
            //     00
            //   00  00
            // 01  01  01
            //   1B  1B
            //     1B
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, height, 0);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height, 1);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_D0 | SEGMENT_D4, height + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C0, height + 6 + 6, 0x1B);
            PaintUtilForceSetGeneralSupportHeight(session, height, 1);
            break;

        case 2:
            // loc_661D4E
            //     02
            //   17  00
            // 17  02  00
            //   17  00
            //     02
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, height, 0);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height, 2);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_D0, height + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8, height + 6 + 6, 0x17);
            PaintUtilForceSetGeneralSupportHeight(session, height, 2);
            break;

        case 3:
            // loc_661DE3
            //     03
            //   03  03
            // 03  03  03
            //   03  03
            //     03
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC, height + 2, 3);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4, height + 2 + 6, 3);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0, height + 2 + 6 + 6, 3);
            PaintUtilForceSetGeneralSupportHeight(session, height, 3);
            break;

        case 4:
            // loc_661E7C
            //     1E
            //   1E  1E
            // 04  04  04
            //   00  00
            //     00
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C0 | SEGMENT_D0 | SEGMENT_D4, height, 0);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height, 4);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_CC, height + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4, height + 6 + 6, 0x1E);
            PaintUtilForceSetGeneralSupportHeight(session, height, 4);
            break;

        case 5:
            // loc_661F11
            //     1E          ▓▓
            //   1E  1E      ▒▒  ▒▒
            // 05  05  05  ░░  ░░  ░░
            //   1B  1B      ▒▒  ▒▒
            //     1B          ▓▓
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4, height + 6 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_CC, height + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height, 5);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_D0 | SEGMENT_D4, height + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C0, height + 6 + 6, 0x1B);
            PaintUtilForceSetGeneralSupportHeight(session, height, 5);
            break;

        case 6:
            // loc_661FA6
            //     06          ▓▓
            //   06  06      ▓▓  ▒▒
            // 06  06  06  ▓▓  ▒▒  ░░
            //   06  06      ▒▒  ░░
            //     06          ░░
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0, height + 2, 6);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, height + 2 + 6, 6);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4, height + 2 + 6 + 6, 6);
            PaintUtilForceSetGeneralSupportHeight(session, height, 6);
            break;

        case 7:
            // loc_66203F
            //     07          ▓▓
            //   00  17      ▓▓  ▒▒
            // 00  07  17  ▓▓  ▓▓  ░░
            //   00  17      ▓▓  ▒▒
            //     07          ▓▓
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_BC, height + 4, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_CC | SEGMENT_D4, height + 4 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height + 4 + 6 + 6, 7);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_D0 | SEGMENT_B8, height + 4 + 6 + 6, 0);
            PaintUtilForceSetGeneralSupportHeight(session, height, 7);
            break;

        case 8:
            // loc_6620D8
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_C8 | SEGMENT_D0, height, 0);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height, 8);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_CC | SEGMENT_D4, height + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_BC, height + 6 + 6, 0x1D);
            PaintUtilForceSetGeneralSupportHeight(session, height, 8);
            break;

        case 9:
            // loc_66216D
            PaintUtilForceSetGeneralSupportHeight(session, height, 9);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8, height + 2, 9);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, height + 2 + 6, 9);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC, height + 2 + 6 + 6, 9);
            break;

        case 10:
            // loc_662206
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xA);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8, height + 6 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_D0, height + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height, 0xA);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_CC | SEGMENT_D4, height + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_BC, height + 6 + 6, 0x1D);
            break;

        case 11:
            // loc_66229B
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xB);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4, height + 4, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_CC, height + 4 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height + 4 + 6 + 6, 0xB);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_D0 | SEGMENT_D4 | SEGMENT_C0, height + 4 + 6 + 6, 0);
            break;

        case 12:
            // loc_662334
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xC);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0, height + 2, 0xC);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4, height + 2 + 6, 0xC);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC, height + 2 + 6 + 6, 0xC);
            break;

        case 13:
            // loc_6623CD
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xD);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8, height + 4, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_D0, height + 4 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height + 4 + 6 + 6, 0xD);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_CC | SEGMENT_D4 | SEGMENT_BC, height + 4 + 6 + 6, 0);
            break;

        case 14:
            // loc_662466
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xE);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C0, height + 4, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_D0 | SEGMENT_D4, height + 4 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height + 4 + 6 + 6, 0xE);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_CC | SEGMENT_B4, height + 4 + 6 + 6, 0);
            break;

        case 23:
            // loc_6624FF
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_BC, height + 4, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_CC | SEGMENT_D4, height + 4 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height + 4 + 6 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_D0, height + 4 + 6 + 6 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8, height + 4 + 6 + 6 + 6 + 6, 0x17);
            break;

        case 27:
            // loc_6625A0
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4, height + 4, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_CC, height + 4 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height + 4 + 6 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_D0 | SEGMENT_D4, height + 4 + 6 + 6 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C0, height + 4 + 6 + 6 + 6 + 6, 0x1B);
            break;

        case 29:
            // loc_662641
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8, height + 4, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_D0, height + 4 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C0, height + 4 + 6 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_CC | SEGMENT_D4, height + 4 + 6 + 6 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_BC, height + 4 + 6 + 6 + 6 + 6, 0x1D);
            break;

        case 30:
            // loc_6626E2
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C0, height + 4, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_D0 | SEGMENT_D4, height + 4 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B8 | SEGMENT_C4 | SEGMENT_BC, height + 4 + 6 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_C8 | SEGMENT_CC, height + 4 + 6 + 6 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, SEGMENT_B4, height + 4 + 6 + 6 + 6 + 6, 0x1E);
            break;
    }
}
