/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Surface.h"

#include "../../Cheats.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../config/Config.h"
#include "../../core/Numerics.hpp"
#include "../../drawing/Drawing.h"
#include "../../entity/EntityRegistry.h"
#include "../../entity/PatrolArea.h"
#include "../../entity/Peep.h"
#include "../../entity/Staff.h"
#include "../../interface/Colour.h"
#include "../../interface/Viewport.h"
#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../paint/Paint.h"
#include "../../profiling/Profiling.h"
#include "../../ride/TrackDesign.h"
#include "../../sprites.h"
#include "../../world/tile_element/Slope.h"
#include "../../world/tile_element/SurfaceElement.h"
#include "../../world/tile_element/TileElement.h"
#include "../Boundbox.h"
#include "../Paint.SessionFlags.h"
#include "Paint.TileElement.h"
#include "Segment.h"

#include <cassert>
#include <cstring>
#include <iterator>

using namespace OpenRCT2;

// Needed to make the sign appear above footpaths.
static constexpr int16_t ForSaleSignZOffset = 3;

static constexpr uint8_t Byte97B444[] = {
    0, 2, 1, 3, 8, 10, 9, 11, 4, 6, 5, 7, 12, 14, 13, 15, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0, 16, 0, 18, 15, 0,
};

// rct2: 0x97B464, 0x97B474, 0x97B484, 0x97B494
static constexpr CoordsXY viewport_surface_paint_data[][4] = {
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

struct CornerHeight
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
static constexpr CornerHeight corner_heights[] = {
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
static constexpr uint8_t Byte97B524[] = {
    2, 5, 1, 4, 2, 5, 1, 2, 2, 4,
    1, 2, 1, 3, 0, 3, 1, 5, 0,
};

// top left tint
static constexpr uint32_t Byte97B537[] = {
    2, 5, 2, 4, 2, 5, 1, 1, 3, 4,
    3, 2, 1, 2, 0, 3, 1, 5, 0,
};

// top right tint
static constexpr uint8_t Byte97B54A[] = {
    2, 2, 2, 4, 0, 0, 1, 1, 3, 4,
    3, 5, 1, 2, 2, 3, 1, 5, 0,
};

// bottom right tint
static constexpr uint8_t Byte97B55D[] = {
    2, 2, 1, 4, 0, 0, 1, 2, 2, 4,
    1, 5, 1, 3, 2, 3, 1, 5, 0,
};

struct TunnelDescriptor
{
    uint8_t height;
    uint8_t boundBoxLength;
    int16_t boundBoxZOffset;
    TunnelType lowClearanceAlternative;
    uint8_t imageOffset;
};
static constexpr TunnelDescriptor kTunnels[] = {
    { 2, 2, 0,   TunnelType::StandardFlat,        36 },  // TunnelType::StandardFlat
    { 3, 3, 0,   TunnelType::StandardFlat,        40 },  // TunnelType::StandardSlopeStart
    { 3, 5, -32, TunnelType::StandardFlat,        44 },  // TunnelType::StandardSlopeEnd
    { 3, 3, 0,   TunnelType::InvertedFlat,        48 },  // TunnelType::InvertedFlat
    { 4, 4, 0,   TunnelType::InvertedFlat,        52 },  // TunnelType::InvertedSlopeStart
    { 4, 6, -48, TunnelType::InvertedFlat,        56 },  // TunnelType::InvertedSlopeEnd
    { 2, 2, 0,   TunnelType::SquareFlat,          60 },  // TunnelType::SquareFlat
    { 3, 3, 0,   TunnelType::SquareFlat,          64 },  // TunnelType::SquareSlopeStart
    { 3, 5, -32, TunnelType::SquareFlat,          68 },  // TunnelType::SquareSlopeEnd
    { 3, 3, 0,   TunnelType::SquareFlat,          72 },  // TunnelType::InvertedSquare
    { 2, 3, -16, TunnelType::PathAndMiniGolf,     76 },  // TunnelType::PathAndMiniGolf
    { 2, 3, -16, TunnelType::Path11,              80 },  // TunnelType::Path11
    { 2, 3, -16, TunnelType::StandardFlatTo25Deg, 36 },  // TunnelType::StandardFlatTo25Deg
    { 3, 4, -16, TunnelType::InvertedFlatTo25Deg, 48 },  // TunnelType::InvertedFlatTo25Deg
    { 2, 3, -16, TunnelType::SquareFlatTo25Deg,   60 },  // TunnelType::SquareFlatTo25Deg
    { 3, 4, -16, TunnelType::SquareFlatTo25Deg,   72 },  // TunnelType::InvertedSquareFlatTo25Deg
    { 2, 2, 0,   TunnelType::Doors0,              76 },  // TunnelType::Doors0
    { 2, 2, 0,   TunnelType::Doors1,              80 },  // TunnelType::Doors1
    { 2, 2, 0,   TunnelType::Doors2,              84 },  // TunnelType::Doors2
    { 2, 2, 0,   TunnelType::Doors3,              88 },  // TunnelType::Doors3
    { 2, 2, 0,   TunnelType::Doors4,              92 },  // TunnelType::Doors4
    { 2, 2, 0,   TunnelType::Doors5,              96 },  // TunnelType::Doors5
    { 2, 2, 0,   TunnelType::Doors6,              100 }, // TunnelType::Doors6
};

// clang-format on
// tunnel offset

static constexpr uint8_t Byte97B740[] = {
    0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 1, 4, 0,
};

struct TileDescriptor
{
    TileCoordsXY tile_coords;
    const TileElement* tile_element;
    const TerrainSurfaceObject* surfaceObject;
    uint8_t slope;
    CornerHeight corner_heights;
};

struct TileSurfaceBoundaryData
{
    int32_t bit_1;
    int32_t bit_8;
    int32_t bit_4;
    int32_t bit_2;
    uint32_t image[5];
    CoordsXY offset;
    BoundBoxXY Boundbox;
};

static constexpr TileSurfaceBoundaryData _tileSurfaceBoundaries[4] = {
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

static ImageId GetSurfacePattern(const TerrainSurfaceObject* surfaceObject, int32_t offset)
{
    ImageId image;
    if (surfaceObject != nullptr)
    {
        image = ImageId(surfaceObject->PatternBaseImageId + offset);
        if (surfaceObject->Colour != TerrainSurfaceObject::kNoValue)
        {
            image = image.WithPrimary(surfaceObject->Colour);
        }
    }
    return image;
}

static bool SurfaceShouldSmoothSelf(const TerrainSurfaceObject* surfaceObject)
{
    if (surfaceObject == nullptr)
        return false;

    return surfaceObject->Flags & TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_SELF;
}

static bool SurfaceShouldSmooth(const TerrainSurfaceObject* surfaceObject)
{
    if (surfaceObject == nullptr)
        return false;

    return surfaceObject->Flags & TERRAIN_SURFACE_FLAGS::SMOOTH_WITH_OTHER;
}

static ImageId GetEdgeImageWithOffset(const TerrainEdgeObject* edgeObject, uint32_t offset)
{
    ImageId result;
    if (edgeObject != nullptr)
    {
        result = ImageId(edgeObject->BaseImageId + offset);
    }
    return result;
}

static ImageId GetEdgeImage(const TerrainEdgeObject* edgeObject, uint8_t type)
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
        result = GetEdgeImageWithOffset(edgeObject, offsets[type]);
    }
    return result;
}

static ImageId GetTunnelImage(const TerrainEdgeObject* edgeObject, TunnelType type, edge_t edge)
{
    bool hasDoors = false;
    if (edgeObject != nullptr)
    {
        hasDoors = edgeObject->HasDoors && !edgeObject->UsesFallbackImages();
    }

    if (!hasDoors && EnumValue(type) >= kRegularTunnelTypeCount)
        type = TunnelType::StandardFlat;

    ImageId result = GetEdgeImageWithOffset(edgeObject, kTunnels[EnumValue(type)].imageOffset)
                         .WithIndexOffset(edge == EDGE_BOTTOMRIGHT ? 2 : 0);

    return result;
}

static uint8_t ViewportSurfacePaintSetupGetRelativeSlope(const SurfaceElement& surfaceElement, int32_t rotation)
{
    const uint8_t slope = surfaceElement.GetSlope();
    const uint8_t slopeHeight = slope & kTileSlopeDiagonalFlag;
    uint16_t slopeCorners = (slope & kTileSlopeRaisedCornersMask) << rotation;
    slopeCorners = ((slopeCorners >> 4) | slopeCorners) & 0x0F;
    return slopeHeight | slopeCorners;
}

/**
 *  rct2: 0x0065E890, 0x0065E946, 0x0065E9FC, 0x0065EAB2
 */
static void ViewportSurfaceSmoothenEdge(
    PaintSession& session, enum edge_t edge, struct TileDescriptor self, struct TileDescriptor neighbour)
{
    PROFILED_FUNCTION();

    if (neighbour.tile_element == nullptr)
        return;

    // The edge row consists of invisible grass tiles. Do not attempt to smooth with them.
    if (MapIsEdge(self.tile_coords.ToCoordsXY()) || MapIsEdge(neighbour.tile_coords.ToCoordsXY()))
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
            dh = Byte97B524[Byte97B444[self.slope]];
            cl = Byte97B54A[Byte97B444[neighbour.slope]];
            break;

        case EDGE_TOPLEFT:
            dh = Byte97B537[Byte97B444[self.slope]];
            cl = Byte97B55D[Byte97B444[neighbour.slope]];
            break;

        case EDGE_BOTTOMRIGHT:
            dh = Byte97B55D[Byte97B444[self.slope]];
            cl = Byte97B537[Byte97B444[neighbour.slope]];
            break;

        case EDGE_TOPRIGHT:
            dh = Byte97B54A[Byte97B444[self.slope]];
            cl = Byte97B524[Byte97B444[neighbour.slope]];
            break;
    }

    if (self.surfaceObject == neighbour.surfaceObject)
    {
        // same tint
        if (cl == dh)
            return;

        if (!SurfaceShouldSmoothSelf(self.surfaceObject))
            return;
    }
    else
    {
        if (!SurfaceShouldSmooth(self.surfaceObject))
            return;

        if (!SurfaceShouldSmooth(neighbour.surfaceObject))
            return;
    }

    const auto image_id = ImageId(maskImageBase + Byte97B444[self.slope]);

    if (PaintAttachToPreviousPS(session, image_id, 0, 0))
    {
        AttachedPaintStruct* out = session.LastAttachedPS;
        // set content and enable masking
        out->ColourImageId = GetSurfacePattern(neighbour.surfaceObject, cl);
        out->IsMasked = true;
    }
}

static bool TileIsInsideClipView(const TileDescriptor& tile)
{
    if (tile.tile_element == nullptr)
        return false;

    if (tile.tile_element->GetBaseZ() > gClipHeight * kCoordsZStep)
        return false;

    auto coords = tile.tile_coords.ToCoordsXY();
    if (coords.x < gClipSelectionA.x || coords.x > gClipSelectionB.x)
        return false;
    if (coords.y < gClipSelectionA.y || coords.y > gClipSelectionB.y)
        return false;

    return true;
}

static void ViewportSurfaceDrawTileSideBottom(
    PaintSession& session, enum edge_t edge, uint16_t height, const TerrainEdgeObject* edgeObject, const TileDescriptor& self,
    const TileDescriptor& neighbour, bool isWater)
{
    PROFILED_FUNCTION();

    // From big Z to tiny Z
    height /= kCoordsZPerTinyZ;
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
        neighbourCornerHeight2 = kMinimumLandHeight / 2;
        neighbourCornerHeight1 = kMinimumLandHeight / 2;
    }

    if (isWater && neighbour.tile_element != nullptr)
    {
        auto waterHeight = neighbour.tile_element->AsSurface()->GetWaterHeight() / (kCoordsZStep * 2);
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

    auto baseImageId = GetEdgeImage(edgeObject, 0);
    if (session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
    {
        baseImageId = GetEdgeImage(edgeObject, 1);
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
            PaintAddImageAsParent(session, imageId, { offset, curHeight * kCoordsZPerTinyZ }, { bounds, 15 });
            curHeight++;
        }
    }

    neighbourCornerHeight1 = cornerHeight2;

    for (auto tunnelIndex = 0; tunnelIndex < kTunnelMaxCount;)
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
            PaintAddImageAsParent(session, imageId, { offset, curHeight * kCoordsZPerTinyZ }, { bounds, 15 });

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
                PaintAddImageAsParent(session, baseImageId, { offset, curHeight * kCoordsZPerTinyZ }, { bounds, 15 });

                curHeight++;
                continue;
            }
        }

        // Tunnels
        auto tunnelType = tunnelArray[tunnelIndex].type;
        auto td = kTunnels[EnumValue(tunnelType)];
        uint8_t tunnelHeight = td.height;
        int16_t zOffset = curHeight;

        // When dealing with flat land but a sloped track, we fall back to the non-sloped variant.
        if ((zOffset + tunnelHeight) > neighbourCornerHeight1 || (zOffset + tunnelHeight) > cornerHeight1)
        {
            tunnelType = td.lowClearanceAlternative;
            td = kTunnels[EnumValue(tunnelType)];
        }

        zOffset *= 16;

        int16_t boundBoxOffsetZ = zOffset + td.boundBoxZOffset;
        int8_t boundBoxLength = td.boundBoxLength * 16;
        if (boundBoxOffsetZ < 16)
        {
            boundBoxOffsetZ += 16;
            boundBoxLength -= 16;
        }

        auto imageId = GetTunnelImage(edgeObject, tunnelType, edge);
        PaintAddImageAsParent(
            session, imageId, { offset, zOffset }, { { 0, 0, boundBoxOffsetZ }, { tunnelBounds, boundBoxLength - 1 } });

        boundBoxOffsetZ = curHeight * kCoordsZPerTinyZ;
        boundBoxLength = td.boundBoxLength * 16;
        boundBoxOffsetZ += td.boundBoxZOffset;
        if (boundBoxOffsetZ == 0)
        {
            boundBoxOffsetZ += 16;
            boundBoxLength -= 16;
        }

        imageId = GetTunnelImage(edgeObject, tunnelType, edge).WithIndexOffset(1);
        PaintAddImageAsParent(
            session, imageId, { offset, curHeight * kCoordsZPerTinyZ },
            { { tunnelTopBoundBoxOffset, boundBoxOffsetZ }, { tunnelBounds, boundBoxLength - 1 } });

        curHeight += td.height;
        tunnelIndex++;
    }
}

static void ViewportSurfaceDrawTileSideTop(
    PaintSession& session, enum edge_t edge, uint16_t height, const TerrainEdgeObject* edgeObject, const TileDescriptor& self,
    const TileDescriptor& neighbour, bool isWater)
{
    PROFILED_FUNCTION();

    // From big Z to tiny Z
    height /= kCoordsZPerTinyZ;

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
            auto waterHeight = neighbour.tile_element->AsSurface()->GetWaterHeight() / (kCoordsZStep * 2);
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
        baseImageId = GetEdgeImage(edgeObject, 2); // var_08
        if (session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
        {
            baseImageId = GetEdgeImage(edgeObject, 1); // var_04
        }
        baseImageId = baseImageId.WithIndexOffset(edge == EDGE_TOPLEFT ? 5 : 0);
    }
    else
    {
        if (!(session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE))
        {
            const uint8_t incline = (cornerHeight2 - cornerHeight1) + 1;
            const auto imageId = GetEdgeImage(edgeObject, 3).WithIndexOffset((edge == EDGE_TOPLEFT ? 3 : 0) + incline);
            const int16_t y = (height - cornerHeight1) * kCoordsZPerTinyZ;
            PaintAttachToPreviousPS(session, imageId, 0, y);
            return;
        }
        baseImageId = GetEdgeImage(edgeObject, 1).WithIndexOffset(edge == EDGE_TOPLEFT ? 5 : 0);
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
                session, imageId, { offset.x, offset.y, cur_height * kCoordsZPerTinyZ }, { bounds.x, bounds.y, 15 });
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
        PaintAddImageAsParent(session, baseImageId, { offset, cur_height * kCoordsZPerTinyZ }, { bounds, 15 });
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
    PaintAddImageAsParent(session, imageId, { offset, cur_height * kCoordsZPerTinyZ }, { bounds, 15 });
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
            localHeight += kLandHeightStep;

            if (waterHeight != localHeight || !(localSurfaceShape & static_cast<int32_t>(kTileSlopeDiagonalFlag)))
            {
                localHeight = waterHeight;
                localSurfaceShape = kTileSlopeFlat;
            }
            else
            {
                localSurfaceShape = Numerics::ror4(surfaceShape ^ static_cast<int8_t>(kTileSlopeRaisedCornersMask), 2);
            }
        }
    }

    return { localHeight, localSurfaceShape };
}

std::optional<colour_t> GetPatrolAreaTileColour(const CoordsXY& pos)
{
    bool selected = gMapSelectFlags & MAP_SELECT_FLAG_ENABLE && gMapSelectType == MAP_SELECT_TYPE_FULL
        && pos.x >= gMapSelectPositionA.x && pos.x <= gMapSelectPositionB.x && pos.y >= gMapSelectPositionA.y
        && pos.y <= gMapSelectPositionB.y;

    auto patrolAreaToRender = GetPatrolAreaToRender();
    if (const auto* staffType = std::get_if<StaffType>(&patrolAreaToRender))
    {
        if (IsPatrolAreaSetForStaffType(*staffType, pos))
        {
            return selected ? COLOUR_WHITE : COLOUR_GREY;
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
                return selected ? COLOUR_ICY_BLUE : COLOUR_LIGHT_BLUE;
            }
            else if (IsPatrolAreaSetForStaffType(staff->AssignedStaffType, pos))
            {
                return selected ? COLOUR_WHITE : COLOUR_GREY;
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
        assert(surfaceShape < std::size(Byte97B444));

        auto [localZ, localSurfaceShape] = SurfaceGetHeightAboveWater(element, height, surfaceShape);
        auto imageId = ImageId(SPR_TERRAIN_SELECTION_PATROL_AREA + Byte97B444[localSurfaceShape], *colour);

        auto* backup = session.LastPS;
        PaintAddImageAsParent(session, imageId, { 0, 0, localZ }, { 32, 32, 1 });
        session.LastPS = backup;
    }
}

static void PaintSurfaceLandOwnership(
    PaintSession& session, const SurfaceElement& tileElement, uint16_t height, uint8_t surfaceShape)
{
    auto [aboveWaterHeight, aboveWaterSurfaceShape] = SurfaceGetHeightAboveWater(tileElement, height, surfaceShape);

    // Loc660E9A:
    if (tileElement.GetOwnership() & OWNERSHIP_OWNED)
    {
        assert(static_cast<size_t>(surfaceShape) < std::size(Byte97B444));
        assert(static_cast<size_t>(aboveWaterSurfaceShape) < std::size(Byte97B444));

        // Paint outline on top of surface (can be underwater)
        PaintAttachToPreviousPS(session, ImageId(SPR_TERRAIN_SELECTION_SQUARE + Byte97B444[surfaceShape]), 0, 0);

        const auto tileIsUnderWater = height != aboveWaterHeight || surfaceShape != aboveWaterSurfaceShape;
        if (tileIsUnderWater)
        {
            PaintStruct* backup = session.LastPS;
            PaintAddImageAsParent(
                session, ImageId(SPR_TERRAIN_SELECTION_SQUARE + Byte97B444[aboveWaterSurfaceShape]), { 0, 0, aboveWaterHeight },
                { 32, 32, 1 });
            session.LastPS = backup;
        }
    }
    else if (tileElement.GetOwnership() & OWNERSHIP_AVAILABLE)
    {
        const auto pos = CoordsXYZ(session.MapPosition.x + 16, session.MapPosition.y + 16, aboveWaterHeight);
        const auto height2 = TileElementHeight(pos, aboveWaterSurfaceShape) + ForSaleSignZOffset;

        PaintStruct* backup = session.LastPS;
        PaintAddImageAsParent(session, ImageId(SPR_LAND_OWNERSHIP_AVAILABLE), { 16, 16, height2 }, { 1, 1, 0 });
        session.LastPS = backup;
    }
}

static void PaintSurfaceConstructionRights(
    PaintSession& session, const SurfaceElement& tileElement, uint16_t height, uint8_t surfaceShape)
{
    auto [aboveWaterHeight, aboveWaterSurfaceShape] = SurfaceGetHeightAboveWater(tileElement, height, surfaceShape);

    if (tileElement.GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
    {
        assert(static_cast<size_t>(surfaceShape) < std::size(Byte97B444));
        assert(static_cast<size_t>(aboveWaterSurfaceShape) < std::size(Byte97B444));

        // Paint outline on top of surface (can be underwater)
        PaintAttachToPreviousPS(session, ImageId(SPR_TERRAIN_SELECTION_DOTTED + Byte97B444[surfaceShape]), 0, 0);

        const auto tileIsUnderWater = height != aboveWaterHeight || surfaceShape != aboveWaterSurfaceShape;
        if (tileIsUnderWater)
        {
            PaintStruct* backup = session.LastPS;
            PaintAddImageAsParent(
                session, ImageId(SPR_TERRAIN_SELECTION_DOTTED + Byte97B444[aboveWaterSurfaceShape]), { 0, 0, aboveWaterHeight },
                { 32, 32, 1 });
            session.LastPS = backup;
        }
    }
    else if (tileElement.GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE)
    {
        const auto pos = CoordsXYZ(session.MapPosition.x + 16, session.MapPosition.y + 16, aboveWaterHeight);
        const auto height2 = TileElementHeight(pos, aboveWaterSurfaceShape) + ForSaleSignZOffset;

        PaintStruct* backup = session.LastPS;
        PaintAddImageAsParent(session, ImageId(SPR_LAND_CONSTRUCTION_RIGHTS_AVAILABLE), { 16, 16, height2 }, { 1, 1, 0 });
        session.LastPS = backup;
    }
}

/**
 *  rct2: 0x0066062C
 */
void PaintSurface(PaintSession& session, uint8_t direction, uint16_t height, const SurfaceElement& tileElement)
{
    PROFILED_FUNCTION();

    session.InteractionType = ViewportInteractionItem::Terrain;
    session.Flags |= PaintSessionFlags::PassedSurface;
    session.Surface = &tileElement;

    const auto zoomLevel = session.DPI.zoom_level;
    const uint8_t rotation = session.CurrentRotation;
    const uint8_t surfaceShape = ViewportSurfacePaintSetupGetRelativeSlope(tileElement, rotation);
    const CoordsXY& base = session.SpritePosition;
    const CornerHeight& cornerHeights = corner_heights[surfaceShape];
    const TileElement* elementPtr = &reinterpret_cast<const TileElement&>(tileElement);

    const auto* surfaceObject = tileElement.GetSurfaceObject();
    const auto* edgeObject = tileElement.GetEdgeObject();

    const auto selfDescriptor = TileDescriptor{
        TileCoordsXY(base),
        elementPtr,
        surfaceObject,
        surfaceShape,
        {
            static_cast<uint8_t>(height / 16 + cornerHeights.top),
            static_cast<uint8_t>(height / 16 + cornerHeights.right),
            static_cast<uint8_t>(height / 16 + cornerHeights.bottom),
            static_cast<uint8_t>(height / 16 + cornerHeights.left),
        },
    };

    TileDescriptor tileDescriptors[4];

    for (std::size_t i = 0; i < std::size(viewport_surface_paint_data); i++)
    {
        const CoordsXY& offset = viewport_surface_paint_data[i][rotation];
        const CoordsXY position = base + offset;

        TileDescriptor& descriptor = tileDescriptors[i];

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
        const CornerHeight& ch = corner_heights[surfaceSlope];

        descriptor.tile_coords = TileCoordsXY{ position };
        descriptor.tile_element = reinterpret_cast<TileElement*>(surfaceElement);
        descriptor.surfaceObject = surfaceElement->GetSurfaceObject();
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
        image_id += GetHeightMarkerOffset();
        image_id -= kMapBaseZ;

        PaintAddImageAsParent(session, ImageId(image_id, COLOUR_OLIVE_GREEN), { 16, 16, surfaceHeight }, { 1, 1, 0 });
    }

    bool has_surface = false;
    if (session.VerticalTunnelHeight * kCoordsZPerTinyZ == height)
    {
        // Vertical tunnels
        PaintAddImageAsParent(session, ImageId(1575), { 0, 0, height }, { { -2, 1, height - 40 }, { 1, 30, 39 } });
        PaintAddImageAsParent(session, ImageId(1576), { 0, 0, height }, { { 1, 31, height }, { 30, 1, 0 } });
        PaintAddImageAsParent(session, ImageId(1577), { 0, 0, height }, { { 31, 1, height }, { 1, 30, 0 } });
        PaintAddImageAsParent(session, ImageId(1578), { 0, 0, height }, { { 1, -2, height - 40 }, { 30, 1, 39 } });
    }
    else
    {
        const bool showGridlines = (session.ViewFlags & VIEWPORT_FLAG_GRIDLINES);

        assert(surfaceShape < std::size(Byte97B444));
        const uint8_t image_offset = Byte97B444[surfaceShape];

        ImageId imageId;
        if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        {
            imageId = ImageId(SPR_TERRAIN_TRACK_DESIGNER);
        }
        else if (surfaceObject != nullptr)
        {
            uint8_t grassLength = TerrainSurfaceObject::kNoValue;
            if (zoomLevel <= ZoomLevel{ 0 })
            {
                if ((session.ViewFlags & (VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_UNDERGROUND_INSIDE)) == 0)
                {
                    grassLength = tileElement.GetGrassLength() & 0x7;
                }
            }
            imageId = surfaceObject->GetImageId(session.MapPosition, grassLength, rotation, image_offset, showGridlines, false);
        }
        if (session.ViewFlags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE))
        {
            imageId = imageId.WithTransparency(FilterPaletteID::PaletteDarken1);
        }

        if (session.SelectedElement == elementPtr)
        {
            imageId = imageId.WithRemap(FilterPaletteID::PaletteGhost);
        }

        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, -1 });
        has_surface = true;
    }

    PaintPatrolArea(session, tileElement, height, surfaceShape);

    auto& gameState = GetGameState();
    // Draw Peep Spawns
    if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gameState.Cheats.sandboxMode)
        && session.ViewFlags & VIEWPORT_FLAG_LAND_OWNERSHIP)
    {
        const CoordsXY& pos = session.MapPosition;
        for (auto& spawn : gameState.PeepSpawns)
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
        PaintSurfaceLandOwnership(session, tileElement, height, surfaceShape);
    }

    if (session.ViewFlags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS && !(tileElement.GetOwnership() & OWNERSHIP_OWNED))
    {
        PaintSurfaceConstructionRights(session, tileElement, height, surfaceShape);
    }

    // ebx[0] = esi;
    // ebp[4] = ebp;
    // ebp[8] = ebx

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
    {
        // Loc660FB8:
        const CoordsXY& pos = session.MapPosition;
        if (pos.x >= gMapSelectPositionA.x && pos.x <= gMapSelectPositionB.x && pos.y >= gMapSelectPositionA.y
            && pos.y <= gMapSelectPositionB.y)
        {
            const uint16_t mapSelectionType = gMapSelectType;
            if (mapSelectionType >= MAP_SELECT_TYPE_EDGE_0)
            {
                // Walls
                // Loc661089:
                const auto fpId = static_cast<FilterPaletteID>(
                    (((mapSelectionType - MAP_SELECT_TYPE_EDGE_0 + 1) + rotation) & 3)
                    + static_cast<uint32_t>(FilterPaletteID::PaletteLandMarker0));
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_EDGE + Byte97B444[surfaceShape], fpId);
                PaintAttachToPreviousPS(session, image_id, 0, 0);
            }
            else if (mapSelectionType >= MAP_SELECT_TYPE_QUARTER_0)
            {
                // Loc661051:(no jump)
                // Selection split into four quarter segments
                const auto fpId = static_cast<FilterPaletteID>(
                    (((mapSelectionType - MAP_SELECT_TYPE_QUARTER_0) + rotation) & 3)
                    + static_cast<uint32_t>(FilterPaletteID::PaletteQuarterMarker0));
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_QUARTER + Byte97B444[surfaceShape], fpId);
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

                const auto fpId = static_cast<FilterPaletteID>(
                    eax + static_cast<uint32_t>(FilterPaletteID::PaletteLandMarker0));
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_CORNER + Byte97B444[surfaceShape], fpId);
                PaintAttachToPreviousPS(session, image_id, 0, 0);
            }
            else if (mapSelectionType == MAP_SELECT_TYPE_FULL_LAND_RIGHTS)
            {
                auto [waterHeight, waterSurfaceShape] = SurfaceGetHeightAboveWater(tileElement, height, surfaceShape);

                const auto fpId = FilterPaletteID::PaletteGlassLightPurple;
                const auto imageId1 = ImageId(SPR_TERRAIN_SELECTION_CORNER + Byte97B444[surfaceShape], fpId);
                PaintAttachToPreviousPS(session, imageId1, 0, 0);

                const bool isUnderWater = (surfaceShape != waterSurfaceShape || height != waterHeight);
                if (isUnderWater)
                {
                    const auto imageId2 = ImageId(SPR_TERRAIN_SELECTION_CORNER + Byte97B444[waterSurfaceShape], fpId);
                    PaintStruct* backup = session.LastPS;
                    PaintAddImageAsParent(session, imageId2, { 0, 0, waterHeight }, { 32, 32, 1 });
                    session.LastPS = backup;
                }
            }
            else
            {
                // The water tool should draw its grid _on_ the water, rather than on the surface under water.
                auto [local_height, local_surfaceShape] = SurfaceGetHeightAboveWater(tileElement, height, surfaceShape);

                const auto fpId = FilterPaletteID::PaletteWaterMarker;
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_CORNER + Byte97B444[local_surfaceShape], fpId);

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

            FilterPaletteID fpId = FilterPaletteID::PaletteSceneryGroundMarker;
            if (gMapSelectFlags & MAP_SELECT_FLAG_GREEN)
            {
                fpId = FilterPaletteID::PaletteRideGroundMarker;
            }

            const auto image_id = ImageId(SPR_TERRAIN_SELECTION_CORNER + Byte97B444[surfaceShape], fpId);
            PaintAttachToPreviousPS(session, image_id, 0, 0);

            auto [waterHeight, waterSurfaceShape] = SurfaceGetHeightAboveWater(tileElement, height, surfaceShape);
            const bool isUnderWater = (surfaceShape != waterSurfaceShape || height != waterHeight);
            if (isUnderWater)
            {
                const auto imageId2 = ImageId(SPR_TERRAIN_SELECTION_CORNER + Byte97B444[waterSurfaceShape], fpId);
                PaintStruct* backup = session.LastPS;
                PaintAddImageAsParent(session, imageId2, { 0, 0, waterHeight }, { 32, 32, 0 });
                session.LastPS = backup;
            }

            break;
        }
    }

    if (zoomLevel <= ZoomLevel{ 0 } && has_surface && !(session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
        && !(session.ViewFlags & VIEWPORT_FLAG_HIDE_BASE) && Config::Get().general.LandscapeSmoothing)
    {
        ViewportSurfaceSmoothenEdge(session, EDGE_TOPLEFT, selfDescriptor, tileDescriptors[2]);
        ViewportSurfaceSmoothenEdge(session, EDGE_TOPRIGHT, selfDescriptor, tileDescriptors[3]);
        ViewportSurfaceSmoothenEdge(session, EDGE_BOTTOMLEFT, selfDescriptor, tileDescriptors[0]);
        ViewportSurfaceSmoothenEdge(session, EDGE_BOTTOMRIGHT, selfDescriptor, tileDescriptors[1]);
    }

    if ((session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE) && !(session.ViewFlags & VIEWPORT_FLAG_HIDE_BASE)
        && !(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        const uint8_t image_offset = Byte97B444[surfaceShape];
        ImageId imageId;
        if (surfaceObject != nullptr)
            imageId = surfaceObject->GetImageId(session.MapPosition, 1, rotation, image_offset, false, true);
        PaintAttachToPreviousPS(session, imageId, 0, 0);
    }

    if (!(session.ViewFlags & VIEWPORT_FLAG_HIDE_VERTICAL))
    {
        ViewportSurfaceDrawTileSideTop(session, EDGE_TOPLEFT, height, edgeObject, selfDescriptor, tileDescriptors[2], false);
        ViewportSurfaceDrawTileSideTop(session, EDGE_TOPRIGHT, height, edgeObject, selfDescriptor, tileDescriptors[3], false);
        ViewportSurfaceDrawTileSideBottom(
            session, EDGE_BOTTOMLEFT, height, edgeObject, selfDescriptor, tileDescriptors[0], false);
        ViewportSurfaceDrawTileSideBottom(
            session, EDGE_BOTTOMRIGHT, height, edgeObject, selfDescriptor, tileDescriptors[1], false);
    }

    const uint16_t waterHeight = tileElement.GetWaterHeight();
    const bool waterGetsClipped = (session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW) && (waterHeight > gClipHeight * kCoordsZStep);

    if (waterHeight > 0 && !gTrackDesignSaveMode && !waterGetsClipped)
    {
        // Loc6615A9: (water height)
        session.InteractionType = ViewportInteractionItem::Water;

        const uint16_t localHeight = height + 16;

        session.WaterHeight = waterHeight;

        int32_t image_offset = 0;
        if (waterHeight <= localHeight)
        {
            image_offset = Byte97B740[surfaceShape & 0xF];
        }

        const auto image_id = ImageId(SPR_WATER_MASK + image_offset, FilterPaletteID::PaletteWater).WithBlended(true);
        PaintAddImageAsParent(session, image_id, { 0, 0, waterHeight }, { 32, 32, -1 });

        const bool transparent = Config::Get().general.TransparentWater
            || (session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE);
        const uint32_t overlayStart = transparent ? EnumValue(SPR_WATER_OVERLAY) : EnumValue(SPR_G2_OPAQUE_WATER_OVERLAY);
        PaintAttachToPreviousPS(session, ImageId(overlayStart + image_offset), 0, 0);

        if (!(session.ViewFlags & VIEWPORT_FLAG_HIDE_VERTICAL))
        {
            ViewportSurfaceDrawTileSideBottom(
                session, EDGE_BOTTOMLEFT, waterHeight, edgeObject, selfDescriptor, tileDescriptors[0], true);
            ViewportSurfaceDrawTileSideBottom(
                session, EDGE_BOTTOMRIGHT, waterHeight, edgeObject, selfDescriptor, tileDescriptors[1], true);
            ViewportSurfaceDrawTileSideTop(
                session, EDGE_TOPLEFT, waterHeight, edgeObject, selfDescriptor, tileDescriptors[2], true);
            ViewportSurfaceDrawTileSideTop(
                session, EDGE_TOPRIGHT, waterHeight, edgeObject, selfDescriptor, tileDescriptors[3], true);
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
            { // Loc6619A2:
                image_id = fenceData.image[1];
            }
            else
            {
                local_height += 16;

                if (!(surfaceShape & 0x10))
                { // Loc6619B5 (first)
                    image_id = fenceData.image[0];
                }
                else if (surfaceShape & fenceData.bit_4)
                { // Loc6619B5 (second)
                    image_id = fenceData.image[3];
                }
                else if (surfaceShape & fenceData.bit_2)
                { // Loc6619B5 (third)
                    image_id = fenceData.image[4];
                }
                else
                {
                    image_id = fenceData.image[0];
                }
            }

            PaintAddImageAsParent(
                session, ImageId(image_id), { fenceData.offset, local_height },
                { { fenceData.Boundbox.offset, local_height + 1 }, { fenceData.Boundbox.length, 9 } });
        }
    }

    session.InteractionType = ViewportInteractionItem::Terrain;

    switch (surfaceShape)
    {
        default:
            // Loc661C2C
            //     00
            //   00  00
            // 00  00  00
            //   00  00
            //     00
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, height, 0);
            PaintUtilForceSetGeneralSupportHeight(session, height, 0);
            break;

        case 1:
            // Loc661CB9
            //     00
            //   00  00
            // 01  01  01
            //   1B  1B
            //     1B
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), height,
                0);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::rightCorner), height, 1);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide), height + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottomCorner), height + 6 + 6, 0x1B);
            PaintUtilForceSetGeneralSupportHeight(session, height, 1);
            break;

        case 2:
            // Loc661D4E
            //     02
            //   17  00
            // 17  02  00
            //   17  00
            //     02
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                height, 0);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::centre, PaintSegment::bottomCorner), height, 2);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::bottomLeftSide), height + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::leftCorner), height + 6 + 6, 0x17);
            PaintUtilForceSetGeneralSupportHeight(session, height, 2);
            break;

        case 3:
            // Loc661DE3
            //     03
            //   03  03
            // 03  03  03
            //   03  03
            //     03
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::topRightSide, PaintSegment::rightCorner),
                height + 2, 3);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide),
                height + 2 + 6, 3);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::bottomLeftSide, PaintSegment::bottomCorner),
                height + 2 + 6 + 6, 3);
            PaintUtilForceSetGeneralSupportHeight(session, height, 3);
            break;

        case 4:
            // Loc661E7C
            //     1E
            //   1E  1E
            // 04  04  04
            //   00  00
            //     00
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomCorner, PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                height, 0);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::rightCorner), height, 4);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::topRightSide), height + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::topCorner), height + 6 + 6, 0x1E);
            PaintUtilForceSetGeneralSupportHeight(session, height, 4);
            break;

        case 5:
            // Loc661F11
            //     1E          ▓▓
            //   1E  1E      ▒▒  ▒▒
            // 05  05  05  ░░  ░░  ░░
            //   1B  1B      ▒▒  ▒▒
            //     1B          ▓▓
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::topCorner), height + 6 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::topRightSide), height + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::rightCorner), height, 5);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide), height + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottomCorner), height + 6 + 6, 0x1B);
            PaintUtilForceSetGeneralSupportHeight(session, height, 5);
            break;

        case 6:
            // Loc661FA6
            //     06          ▓▓
            //   06  06      ▓▓  ▒▒
            // 06  06  06  ▓▓  ▒▒  ░░
            //   06  06      ▒▒  ░░
            //     06          ░░
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::rightCorner, PaintSegment::bottomRightSide, PaintSegment::bottomCorner),
                height + 2, 6);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide),
                height + 2 + 6, 6);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::topLeftSide, PaintSegment::topCorner),
                height + 2 + 6 + 6, 6);
            PaintUtilForceSetGeneralSupportHeight(session, height, 6);
            break;

        case 7:
            // Loc66203F
            //     07          ▓▓
            //   00  17      ▓▓  ▒▒
            // 00  07  17  ▓▓  ▓▓  ░░
            //   00  17      ▓▓  ▒▒
            //     07          ▓▓
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::rightCorner), height + 4, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRightSide, PaintSegment::bottomRightSide), height + 4 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::centre, PaintSegment::bottomCorner),
                height + 4 + 6 + 6, 7);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::bottomLeftSide, PaintSegment::leftCorner),
                height + 4 + 6 + 6, 0);
            PaintUtilForceSetGeneralSupportHeight(session, height, 7);
            break;

        case 8:
            // Loc6620D8
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide),
                height, 0);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::centre, PaintSegment::bottomCorner), height, 8);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRightSide, PaintSegment::bottomRightSide), height + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::rightCorner), height + 6 + 6, 0x1D);
            PaintUtilForceSetGeneralSupportHeight(session, height, 8);
            break;

        case 9:
            // Loc66216D
            PaintUtilForceSetGeneralSupportHeight(session, height, 9);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::leftCorner), height + 2,
                9);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide),
                height + 2 + 6, 9);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomCorner, PaintSegment::bottomRightSide, PaintSegment::rightCorner),
                height + 2 + 6 + 6, 9);
            break;

        case 10:
            // Loc662206
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xA);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::leftCorner), height + 6 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::bottomLeftSide), height + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::centre, PaintSegment::bottomCorner), height, 0xA);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRightSide, PaintSegment::bottomRightSide), height + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::rightCorner), height + 6 + 6, 0x1D);
            break;

        case 11:
            // Loc66229B
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xB);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::topCorner), height + 4, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::topRightSide), height + 4 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::rightCorner),
                height + 4 + 6 + 6, 0xB);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide, PaintSegment::bottomCorner),
                height + 4 + 6 + 6, 0);
            break;

        case 12:
            // Loc662334
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xC);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::bottomLeftSide, PaintSegment::bottomCorner),
                height + 2, 0xC);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide),
                height + 2 + 6, 0xC);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::topRightSide, PaintSegment::rightCorner),
                height + 2 + 6 + 6, 0xC);
            break;

        case 13:
            // Loc6623CD
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xD);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::leftCorner), height + 4, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::bottomLeftSide), height + 4 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::centre, PaintSegment::bottomCorner),
                height + 4 + 6 + 6, 0xD);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRightSide, PaintSegment::bottomRightSide, PaintSegment::rightCorner),
                height + 4 + 6 + 6, 0);
            break;

        case 14:
            // Loc662466
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xE);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottomCorner), height + 4, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide), height + 4 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::rightCorner),
                height + 4 + 6 + 6, 0xE);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::topCorner),
                height + 4 + 6 + 6, 0);
            break;

        case 23:
            // Loc6624FF
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x17);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::rightCorner), height + 4, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRightSide, PaintSegment::bottomRightSide), height + 4 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::centre, PaintSegment::bottomCorner),
                height + 4 + 6 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::bottomLeftSide), height + 4 + 6 + 6 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::leftCorner), height + 4 + 6 + 6 + 6 + 6, 0x17);
            break;

        case 27:
            // Loc6625A0
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::topCorner), height + 4, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::topRightSide), height + 4 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::rightCorner),
                height + 4 + 6 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide), height + 4 + 6 + 6 + 6,
                0x1B);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottomCorner), height + 4 + 6 + 6 + 6 + 6, 0x1B);
            break;

        case 29:
            // Loc662641
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::leftCorner), height + 4, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::bottomLeftSide), height + 4 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topCorner, PaintSegment::centre, PaintSegment::bottomCorner),
                height + 4 + 6 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRightSide, PaintSegment::bottomRightSide), height + 4 + 6 + 6 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::rightCorner), height + 4 + 6 + 6 + 6 + 6, 0x1D);
            break;

        case 30:
            // Loc6626E2
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottomCorner), height + 4, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide), height + 4 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::rightCorner),
                height + 4 + 6 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::topRightSide), height + 4 + 6 + 6 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::topCorner), height + 4 + 6 + 6 + 6 + 6, 0x1E);
            break;
    }
}
