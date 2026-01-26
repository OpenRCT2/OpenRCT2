/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../../SpriteIds.h"
#include "../../config/Config.h"
#include "../../core/Numerics.hpp"
#include "../../entity/EntityRegistry.h"
#include "../../entity/PatrolArea.h"
#include "../../entity/Peep.h"
#include "../../entity/Staff.h"
#include "../../interface/Viewport.h"
#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../paint/Paint.h"
#include "../../profiling/Profiling.h"
#include "../../ride/TrackDesign.h"
#include "../../world/Map.h"
#include "../../world/MapSelection.h"
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
using namespace OpenRCT2::Drawing;

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

// clang-format off

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
    int8_t lowerEdgeBoundingBoxZ;
    TunnelType lowClearanceAlternative;
    uint8_t imageOffset;
};
static constexpr TunnelDescriptor kTunnels[] = {
    { 2, 2, 0,   15, TunnelType::StandardFlat,                    36 }, // TunnelType::StandardFlat
    { 3, 3, 0,   15, TunnelType::StandardFlat,                    40 }, // TunnelType::StandardSlopeStart
    { 3, 5, -32,  4, TunnelType::StandardFlat,                    44 }, // TunnelType::StandardSlopeEnd
    { 3, 3, 0,   15, TunnelType::InvertedFlat,                    48 }, // TunnelType::InvertedFlat
    { 4, 4, 0,   15, TunnelType::InvertedFlat,                    52 }, // TunnelType::InvertedSlopeStart
    { 4, 7, -48,  4, TunnelType::InvertedFlat,                    56 }, // TunnelType::InvertedSlopeEnd
    { 2, 2, 0,   15, TunnelType::SquareFlat,                      60 }, // TunnelType::SquareFlat
    { 3, 3, 0,   15, TunnelType::SquareFlat,                      64 }, // TunnelType::SquareSlopeStart
    { 3, 5, -32,  4, TunnelType::SquareFlat,                      68 }, // TunnelType::SquareSlopeEnd
    { 3, 3, 0,   15, TunnelType::SquareFlat,                      72 }, // TunnelType::InvertedSquare
    { 2, 3, -16, 15, TunnelType::PathAndMiniGolf,                 76 }, // TunnelType::PathAndMiniGolf
    { 2, 3, -16, 15, TunnelType::Path11,                          80 }, // TunnelType::Path11
    { 2, 3, -16,  4, TunnelType::StandardFlatTo25Deg,             36 }, // TunnelType::StandardFlatTo25Deg
    { 3, 4, -16,  4, TunnelType::InvertedFlatTo25Deg,             48 }, // TunnelType::InvertedFlatTo25Deg
    { 2, 3, -16,  4, TunnelType::SquareFlatTo25Deg,               60 }, // TunnelType::SquareFlatTo25Deg
    { 3, 4, -16,  4, TunnelType::SquareFlatTo25Deg,               72 }, // TunnelType::InvertedSquareFlatTo25Deg
    { 2, 2, 0,   15, TunnelType::doorClosed,                      84 }, // TunnelType::doorClosed
    { 2, 2, 0,   15, TunnelType::doorOpeningOutward,              88 }, // TunnelType::doorOpeningOutward
    { 2, 2, 0,   15, TunnelType::doorOpenOutward,                 92 }, // TunnelType::doorOpenOutward
    { 2, 2, 0,   15, TunnelType::doorOpeningInward,               96 }, // TunnelType::doorOpeningInward
    { 2, 2, 0,   15, TunnelType::doorOpenInward,                 100 }, // TunnelType::doorOpenInward
    { 2, 3, -16,  4, TunnelType::doorClosedFlatToDown25,          84 }, // TunnelType::doorClosedFlatToDown25
    { 2, 3, -16,  4, TunnelType::doorOpeningOutwardFlatToDown25,  88 }, // TunnelType::doorOpeningOutwardFlatToDown25
    { 2, 3, -16,  4, TunnelType::doorOpenOutwardFlatToDown25,     92 }, // TunnelType::doorOpenOutwardFlatToDown25
    { 2, 3, -16,  4, TunnelType::doorOpeningInwardFlatToDown25,   96 }, // TunnelType::doorOpeningInwardFlatToDown25
    { 2, 3, -16,  4, TunnelType::doorOpenInwardFlatToDown25,     100 }, // TunnelType::doorOpenInwardFlatToDown25
};
static_assert(std::size(kTunnels) == kTunnelTypeCount);

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
    SlopeRelativeCornerHeights corner_heights;
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
        if (surfaceObject->Colour != kColourNull)
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

    return surfaceObject->Flags.has(TerrainSurfaceFlag::smoothWithSelf);
}

static bool SurfaceShouldSmooth(const TerrainSurfaceObject* surfaceObject)
{
    if (surfaceObject == nullptr)
        return false;

    return surfaceObject->Flags.has(TerrainSurfaceFlag::smoothWithOther);
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
    PaintSession& session, const edge_t edge, const TileDescriptor& self, const TileDescriptor& neighbour)
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

        const int16_t minimumNeighbourCornerHeight = std::min(neighbourCornerHeight1, neighbourCornerHeight2);
        neighbourCornerHeight1 = std::max(cornerHeight1, minimumNeighbourCornerHeight);
        neighbourCornerHeight2 = std::max(cornerHeight2, minimumNeighbourCornerHeight);

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

    const auto lowestCornerHeight = std::min(cornerHeight1, cornerHeight2);

    const auto& tunnels = edge == EDGE_BOTTOMLEFT ? session.LeftTunnels : session.RightTunnels;
    for (auto& tunnel : tunnels)
    {
        if (curHeight > tunnel.height || tunnel.height >= lowestCornerHeight)
        {
            continue;
        }

        const auto tdOriginal = kTunnels[EnumValue(tunnel.type)];

        // Draw land edges up to the bottom of the tunnel
        while (curHeight < tunnel.height)
        {
            const auto boundBoxZ = curHeight == tunnel.height - 1 ? tdOriginal.lowerEdgeBoundingBoxZ : kCoordsZPerTinyZ - 1;
            PaintAddImageAsParent(session, baseImageId, { offset, curHeight * kCoordsZPerTinyZ }, { bounds, boundBoxZ });
            curHeight++;
        }

        auto tunnelType = tunnel.type;
        auto td = tdOriginal;
        uint8_t tunnelHeight = td.height;
        int16_t zOffset = curHeight;

        // When dealing with flat land but a sloped track, we fall back to the non-sloped variant.
        if ((zOffset + tunnelHeight) > neighbourCornerHeight1 || (zOffset + tunnelHeight) > cornerHeight1)
        {
            tunnelType = td.lowClearanceAlternative;
            td = kTunnels[EnumValue(tunnelType)];
        }

        zOffset *= 16;

        const int8_t boundBoxLengthBase = (tdOriginal.boundBoxLength + (td.height - tdOriginal.height)) * 16;

        int16_t boundBoxOffsetZ = zOffset + tdOriginal.boundBoxZOffset;
        int8_t boundBoxLength = boundBoxLengthBase;
        if (boundBoxOffsetZ < 16)
        {
            boundBoxOffsetZ += 16;
            boundBoxLength -= 16;
        }

        auto imageId = GetTunnelImage(edgeObject, tunnelType, edge);
        PaintAddImageAsParent(
            session, imageId, { offset, zOffset }, { { 0, 0, boundBoxOffsetZ }, { tunnelBounds, boundBoxLength - 1 } });

        boundBoxOffsetZ = curHeight * kCoordsZPerTinyZ;
        boundBoxLength = boundBoxLengthBase;
        boundBoxOffsetZ += tdOriginal.boundBoxZOffset;
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
    }

    // Draw land edges up to the lowest corner
    while (curHeight < lowestCornerHeight)
    {
        PaintAddImageAsParent(session, baseImageId, { offset, curHeight * kCoordsZPerTinyZ }, { bounds, kCoordsZPerTinyZ - 1 });
        curHeight++;
    }

    // Draw filler edges if the top edge is not straight
    if (curHeight < cornerHeight1 || curHeight < cornerHeight2)
    {
        const uint32_t imageOffset = curHeight >= cornerHeight1 ? 2 : 1;
        const auto imageId = baseImageId.WithIndexOffset(imageOffset);
        PaintAddImageAsParent(session, imageId, { offset, curHeight * kCoordsZPerTinyZ }, { bounds, kCoordsZPerTinyZ - 1 });
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

            neighbourCornerHeight1 = std::max(cornerHeight1, neighbourCornerHeight1);
            neighbourCornerHeight2 = std::max(cornerHeight2, neighbourCornerHeight2);

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

        offset.x = 0;
        offset.y = 0;
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

std::optional<OpenRCT2::Drawing::Colour> GetPatrolAreaTileColour(const CoordsXY& pos)
{
    bool selected = gMapSelectFlags.has(MapSelectFlag::enable) && gMapSelectType == MapSelectType::full
        && pos.x >= gMapSelectPositionA.x && pos.x <= gMapSelectPositionB.x && pos.y >= gMapSelectPositionA.y
        && pos.y <= gMapSelectPositionB.y;

    auto patrolAreaToRender = GetPatrolAreaToRender();
    if (const auto* staffType = std::get_if<StaffType>(&patrolAreaToRender))
    {
        if (IsPatrolAreaSetForStaffType(*staffType, pos))
        {
            return selected ? OpenRCT2::Drawing::Colour::white : OpenRCT2::Drawing::Colour::grey;
        }
    }
    else
    {
        auto& staffId = std::get<EntityId>(patrolAreaToRender);
        auto* staff = getGameState().entities.GetEntity<Staff>(staffId);
        if (staff != nullptr)
        {
            if (staff->IsPatrolAreaSet(pos))
            {
                return selected ? OpenRCT2::Drawing::Colour::icyBlue : OpenRCT2::Drawing::Colour::lightBlue;
            }
            else if (IsPatrolAreaSetForStaffType(staff->AssignedStaffType, pos))
            {
                return selected ? OpenRCT2::Drawing::Colour::white : OpenRCT2::Drawing::Colour::grey;
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

    session.InteractionType = ViewportInteractionItem::terrain;
    session.Flags |= PaintSessionFlags::PassedSurface;
    session.Surface = &tileElement;

    const auto zoomLevel = session.rt.zoom_level;
    const uint8_t rotation = session.CurrentRotation;
    const uint8_t surfaceShape = ViewportSurfacePaintSetupGetRelativeSlope(tileElement, rotation);
    const CoordsXY& base = session.SpritePosition;
    const auto cornerHeights = GetSlopeRelativeCornerHeights(surfaceShape);
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
        const auto ch = GetSlopeRelativeCornerHeights(surfaceSlope);

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

        PaintAddImageAsParent(
            session, ImageId(image_id, OpenRCT2::Drawing::Colour::oliveGreen), { 16, 16, surfaceHeight }, { 1, 1, 0 });
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
        if (isInTrackDesignerOrManager())
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
            imageId = imageId.WithTransparency(FilterPaletteID::paletteDarken1);
        }

        if (session.SelectedElement == elementPtr)
        {
            imageId = imageId.WithRemap(FilterPaletteID::paletteGhost);
        }

        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, -1 });
        has_surface = true;
    }

    PaintPatrolArea(session, tileElement, height, surfaceShape);

    auto& gameState = getGameState();
    // Draw Peep Spawns
    if ((gLegacyScene == LegacyScene::scenarioEditor || gameState.cheats.sandboxMode)
        && session.ViewFlags & VIEWPORT_FLAG_LAND_OWNERSHIP)
    {
        const CoordsXY& pos = session.MapPosition;
        for (auto& spawn : gameState.peepSpawns)
        {
            if ((spawn.x & 0xFFE0) == pos.x && (spawn.y & 0xFFE0) == pos.y)
            {
                PaintAddImageAsParent(session, ImageId(SPR_TERRAIN_SELECTION_SQUARE_SIMPLE), { 0, 0, spawn.z }, { 32, 32, 16 });

                const int32_t offset = (DirectionReverse(spawn.direction) + rotation) & 3;
                const auto image_id = ImageId(PEEP_SPAWN_ARROW_0 + offset, OpenRCT2::Drawing::Colour::lightBlue);
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

    if (gMapSelectFlags.has(MapSelectFlag::enable))
    {
        // Loc660FB8:
        const CoordsXY& pos = session.MapPosition;
        if (pos.x >= gMapSelectPositionA.x && pos.x <= gMapSelectPositionB.x && pos.y >= gMapSelectPositionA.y
            && pos.y <= gMapSelectPositionB.y)
        {
            const auto mapSelectionType = gMapSelectType;
            if (mapSelectionType >= MapSelectType::edge0)
            {
                // Walls
                // Loc661089:
                const auto fpId = static_cast<FilterPaletteID>(
                    (((EnumValue(mapSelectionType) - EnumValue(MapSelectType::edge0) + 1) + rotation) & 3)
                    + static_cast<uint32_t>(FilterPaletteID::paletteLandMarker0));
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_EDGE + Byte97B444[surfaceShape], fpId);
                PaintAttachToPreviousPS(session, image_id, 0, 0);
            }
            else if (mapSelectionType >= MapSelectType::quarter0)
            {
                // Loc661051:(no jump)
                // Selection split into four quarter segments
                const auto fpId = static_cast<FilterPaletteID>(
                    (((EnumValue(mapSelectionType) - EnumValue(MapSelectType::quarter0)) + rotation) & 3)
                    + static_cast<uint32_t>(FilterPaletteID::paletteQuarterMarker0));
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_QUARTER + Byte97B444[surfaceShape], fpId);
                PaintAttachToPreviousPS(session, image_id, 0, 0);
            }
            else if (mapSelectionType <= MapSelectType::full)
            {
                // Corners
                uint32_t eax = EnumValue(mapSelectionType);
                if (mapSelectionType != MapSelectType::full)
                {
                    eax = (eax + rotation) & 3;
                }

                const auto fpId = static_cast<FilterPaletteID>(
                    eax + static_cast<uint32_t>(FilterPaletteID::paletteLandMarker0));
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_CORNER + Byte97B444[surfaceShape], fpId);
                PaintAttachToPreviousPS(session, image_id, 0, 0);
            }
            else if (mapSelectionType == MapSelectType::fullLandRights)
            {
                auto [waterHeight, waterSurfaceShape] = SurfaceGetHeightAboveWater(tileElement, height, surfaceShape);

                const auto fpId = FilterPaletteID::paletteSceneryGroundMarker;
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

                const auto fpId = FilterPaletteID::paletteWaterMarker;
                const auto image_id = ImageId(SPR_TERRAIN_SELECTION_CORNER + Byte97B444[local_surfaceShape], fpId);

                PaintStruct* backup = session.LastPS;
                PaintAddImageAsParent(session, image_id, { 0, 0, local_height }, { 32, 32, 1 });
                session.LastPS = backup;
            }
        }
    }

    if (gMapSelectFlags.has(MapSelectFlag::enableConstruct))
    {
        const CoordsXY& pos = session.MapPosition;

        for (const auto& tile : MapSelection::getSelectedTiles())
        {
            if (tile.x != pos.x || tile.y != pos.y)
            {
                continue;
            }

            FilterPaletteID fpId = FilterPaletteID::paletteSceneryGroundMarker;
            if (gMapSelectFlags.has(MapSelectFlag::green))
            {
                fpId = FilterPaletteID::paletteRideGroundMarker;
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
        && !(session.ViewFlags & VIEWPORT_FLAG_HIDE_BASE) && Config::Get().general.landscapeSmoothing)
    {
        ViewportSurfaceSmoothenEdge(session, EDGE_TOPLEFT, selfDescriptor, tileDescriptors[2]);
        ViewportSurfaceSmoothenEdge(session, EDGE_TOPRIGHT, selfDescriptor, tileDescriptors[3]);
        ViewportSurfaceSmoothenEdge(session, EDGE_BOTTOMLEFT, selfDescriptor, tileDescriptors[0]);
        ViewportSurfaceSmoothenEdge(session, EDGE_BOTTOMRIGHT, selfDescriptor, tileDescriptors[1]);
    }

    if ((session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE) && !(session.ViewFlags & VIEWPORT_FLAG_HIDE_BASE)
        && !(isInTrackDesignerOrManager()))
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
    const bool waterIsTransparent = Config::Get().general.transparentWater
        || (session.ViewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE);

    if (waterHeight > 0 && !gTrackDesignSaveMode && !waterGetsClipped)
    {
        // Loc6615A9: (water height)
        session.InteractionType = ViewportInteractionItem::water;

        const uint16_t localHeight = height + 16;

        session.WaterHeight = waterHeight;

        int32_t image_offset = 0;
        if (waterHeight <= localHeight)
        {
            image_offset = Byte97B740[surfaceShape & 0xF];
        }

        const auto image_id = ImageId(SPR_WATER_MASK + image_offset, FilterPaletteID::paletteWater).WithBlended(true);
        PaintAddImageAsParent(session, image_id, { 0, 0, waterHeight }, { 32, 32, -1 });

        const uint32_t overlayStart = waterIsTransparent ? EnumValue(SPR_WATER_OVERLAY)
                                                         : EnumValue(SPR_G2_OPAQUE_WATER_OVERLAY);
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
        session.InteractionType = ViewportInteractionItem::parkEntrance;

        uint8_t rotatedFences = Numerics::rol4(tileElement.GetParkFences(), rotation);

        for (const auto& fenceData : _tileSurfaceBoundaries)
        {
            const int32_t edgeHasFence = rotatedFences & 1;
            rotatedFences >>= 1;

            if (edgeHasFence == 0)
                continue;

            int32_t fenceHeight = height;
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
                fenceHeight += 16;

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

            if (!waterIsTransparent && fenceHeight < waterHeight)
            {
                continue;
            }

            PaintAddImageAsParent(
                session, ImageId(image_id), { fenceData.offset, fenceHeight },
                { { fenceData.Boundbox.offset, fenceHeight + 1 }, { fenceData.Boundbox.length, 9 } });
        }
    }

    session.InteractionType = ViewportInteractionItem::terrain;

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
                session, EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), height, 0);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::right), height, 1);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottomRight), height + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottom), height + 6 + 6, 0x1B);
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
                session, EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), height, 0);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::bottom), height, 2);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::bottomLeft), height + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::left), height + 6 + 6, 0x17);
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
                session, EnumsToFlags(PaintSegment::top, PaintSegment::topRight, PaintSegment::right), height + 2, 3);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight), height + 2 + 6,
                3);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::bottom), height + 2 + 6 + 6,
                3);
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
                session, EnumsToFlags(PaintSegment::bottom, PaintSegment::bottomLeft, PaintSegment::bottomRight), height, 0);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::right), height, 4);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::topRight), height + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::top), height + 6 + 6, 0x1E);
            PaintUtilForceSetGeneralSupportHeight(session, height, 4);
            break;

        case 5:
            // Loc661F11
            //     1E          ▓▓
            //   1E  1E      ▒▒  ▒▒
            // 05  05  05  ░░  ░░  ░░
            //   1B  1B      ▒▒  ▒▒
            //     1B          ▓▓
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::top), height + 6 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::topRight), height + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::right), height, 5);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottomRight), height + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottom), height + 6 + 6, 0x1B);
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
                session, EnumsToFlags(PaintSegment::right, PaintSegment::bottomRight, PaintSegment::bottom), height + 2, 6);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), height + 2 + 6,
                6);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::top), height + 2 + 6 + 6, 6);
            PaintUtilForceSetGeneralSupportHeight(session, height, 6);
            break;

        case 7:
            // Loc66203F
            //     07          ▓▓
            //   00  17      ▓▓  ▒▒
            // 00  07  17  ▓▓  ▓▓  ░░
            //   00  17      ▓▓  ▒▒
            //     07          ▓▓
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::right), height + 4, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRight, PaintSegment::bottomRight), height + 4 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::bottom), height + 4 + 6 + 6, 7);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::left), height + 4 + 6 + 6,
                0);
            PaintUtilForceSetGeneralSupportHeight(session, height, 7);
            break;

        case 8:
            // Loc6620D8
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft), height, 0);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::bottom), height, 8);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRight, PaintSegment::bottomRight), height + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::right), height + 6 + 6, 0x1D);
            PaintUtilForceSetGeneralSupportHeight(session, height, 8);
            break;

        case 9:
            // Loc66216D
            PaintUtilForceSetGeneralSupportHeight(session, height, 9);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::left), height + 2, 9);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), height + 2 + 6,
                9);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::right), height + 2 + 6 + 6,
                9);
            break;

        case 10:
            // Loc662206
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xA);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::left), height + 6 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::bottomLeft), height + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::bottom), height, 0xA);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRight, PaintSegment::bottomRight), height + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::right), height + 6 + 6, 0x1D);
            break;

        case 11:
            // Loc66229B
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xB);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::top), height + 4, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::topRight), height + 4 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::right), height + 4 + 6 + 6, 0xB);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottomRight, PaintSegment::bottom),
                height + 4 + 6 + 6, 0);
            break;

        case 12:
            // Loc662334
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xC);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::bottom), height + 2, 0xC);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight), height + 2 + 6,
                0xC);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::top, PaintSegment::topRight, PaintSegment::right), height + 2 + 6 + 6, 0xC);
            break;

        case 13:
            // Loc6623CD
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xD);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::left), height + 4, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::bottomLeft), height + 4 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::bottom), height + 4 + 6 + 6, 0xD);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                height + 4 + 6 + 6, 0);
            break;

        case 14:
            // Loc662466
            PaintUtilForceSetGeneralSupportHeight(session, height, 0xE);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottom), height + 4, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottomRight), height + 4 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::right), height + 4 + 6 + 6, 0xE);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::top), height + 4 + 6 + 6, 0);
            break;

        case 23:
            // Loc6624FF
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x17);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::right), height + 4, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRight, PaintSegment::bottomRight), height + 4 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::bottom), height + 4 + 6 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::bottomLeft), height + 4 + 6 + 6 + 6, 0x17);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::left), height + 4 + 6 + 6 + 6 + 6, 0x17);
            break;

        case 27:
            // Loc6625A0
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::top), height + 4, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::topRight), height + 4 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::right), height + 4 + 6 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottomRight), height + 4 + 6 + 6 + 6, 0x1B);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottom), height + 4 + 6 + 6 + 6 + 6, 0x1B);
            break;

        case 29:
            // Loc662641
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::left), height + 4, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::bottomLeft), height + 4 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::bottom), height + 4 + 6 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topRight, PaintSegment::bottomRight), height + 4 + 6 + 6 + 6, 0x1D);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::right), height + 4 + 6 + 6 + 6 + 6, 0x1D);
            break;

        case 30:
            // Loc6626E2
            PaintUtilForceSetGeneralSupportHeight(session, height, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottom), height + 4, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottomRight), height + 4 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::right), height + 4 + 6 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(
                session, EnumsToFlags(PaintSegment::topLeft, PaintSegment::topRight), height + 4 + 6 + 6 + 6, 0x1E);
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::top), height + 4 + 6 + 6 + 6 + 6, 0x1E);
            break;
    }
}
