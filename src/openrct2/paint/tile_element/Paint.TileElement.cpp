/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.TileElement.h"

#include "../../Game.h"
#include "../../Input.h"
#include "../../config/Config.h"
#include "../../core/Numerics.hpp"
#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../profiling/Profiling.h"
#include "../../ride/RideData.h"
#include "../../ride/TrackData.h"
#include "../../ride/TrackPaint.h"
#include "../../sprites.h"
#include "../../world/Banner.h"
#include "../../world/Entrance.h"
#include "../../world/Footpath.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../../world/Surface.h"
#include "../Paint.SessionFlags.h"
#include "../Paint.h"
#include "../Supports.h"
#include "../VirtualFloor.h"
#include "Paint.Surface.h"

#include <algorithm>

static void BlankTilesPaint(PaintSession& session, int32_t x, int32_t y);
static void PaintTileElementBase(PaintSession& session, const CoordsXY& origCoords);

const int32_t SEGMENTS_ALL = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC
    | SEGMENT_D0 | SEGMENT_D4;

/**
 *
 *  rct2: 0x0068B35F
 */
void TileElementPaintSetup(PaintSession& session, const CoordsXY& mapCoords, bool isTrackPiecePreview)
{
    PROFILED_FUNCTION();

    if (!MapIsEdge(mapCoords))
    {
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilForceSetGeneralSupportHeight(session, -1, 0);
        session.Flags = isTrackPiecePreview ? PaintSessionFlags::IsTrackPiecePreview : 0;
        session.WaterHeight = 0xFFFF;

        PaintTileElementBase(session, mapCoords);
    }
    else if (!(session.ViewFlags & VIEWPORT_FLAG_TRANSPARENT_BACKGROUND))
    {
        BlankTilesPaint(session, mapCoords.x, mapCoords.y);
    }
}

/**
 *
 *  rct2: 0x0068B60E
 */
static void BlankTilesPaint(PaintSession& session, int32_t x, int32_t y)
{
    int32_t dx = 0;
    switch (session.CurrentRotation)
    {
        case 0:
            dx = x + y;
            break;
        case 1:
            x += 32;
            dx = y - x;
            break;
        case 2:
            x += 32;
            y += 32;
            dx = -(x + y);
            break;
        case 3:
            y += 32;
            dx = x - y;
            break;
    }
    dx /= 2;
    dx -= 16;
    int32_t bx = dx + 32;

    rct_drawpixelinfo* dpi = &session.DPI;
    if (bx <= dpi->y)
        return;
    dx -= 20;
    dx -= dpi->height;
    if (dx >= dpi->y)
        return;

    session.SpritePosition.x = x;
    session.SpritePosition.y = y;
    session.InteractionType = ViewportInteractionItem::None;
    PaintAddImageAsParent(session, ImageId(SPR_BLANK_TILE), { 0, 0, 16 }, { 32, 32, -1 });
}

bool gShowSupportSegmentHeights = false;

/**
 *
 *  rct2: 0x0068B3FB
 */
static void PaintTileElementBase(PaintSession& session, const CoordsXY& origCoords)
{
    PROFILED_FUNCTION();

    CoordsXY coords = origCoords;
    rct_drawpixelinfo* dpi = &session.DPI;

    if ((session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW))
    {
        if (coords.x < gClipSelectionA.x || coords.x > gClipSelectionB.x)
            return;
        if (coords.y < gClipSelectionA.y || coords.y > gClipSelectionB.y)
            return;
    }

    session.LeftTunnelCount = 0;
    session.RightTunnelCount = 0;
    session.LeftTunnels[0] = { 0xFF, 0xFF };
    session.RightTunnels[0] = { 0xFF, 0xFF };
    session.VerticalTunnelHeight = 0xFF;
    session.MapPosition.x = coords.x;
    session.MapPosition.y = coords.y;

    auto* tile_element = MapGetFirstElementAt(session.MapPosition);
    if (tile_element == nullptr)
        return;
    uint8_t rotation = session.CurrentRotation;

    bool partOfVirtualFloor = false;

    if (gConfigGeneral.VirtualFloorStyle != VirtualFloorStyles::Off)
    {
        partOfVirtualFloor = VirtualFloorTileIsFloor(session.MapPosition);
    }

    switch (rotation)
    {
        case 0:
            break;
        case 1:
            coords.x += COORDS_XY_STEP;
            break;
        case 2:
            coords.x += COORDS_XY_STEP;
            coords.y += COORDS_XY_STEP;
            break;
        case 3:
            coords.y += COORDS_XY_STEP;
            break;
    }

    int32_t screenMinY = Translate3DTo2DWithZ(rotation, { coords, 0 }).y;

    // Display little yellow arrow when building footpaths?
    if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_ARROW) && session.MapPosition.x == gMapSelectArrowPosition.x
        && session.MapPosition.y == gMapSelectArrowPosition.y)
    {
        uint8_t arrowRotation = (rotation + (gMapSelectArrowDirection & 3)) & 3;

        uint32_t imageIndex = arrowRotation + (gMapSelectArrowDirection & 0xFC) + PEEP_SPAWN_ARROW_0;
        ImageId imageId = ImageId(imageIndex, COLOUR_YELLOW);
        int32_t arrowZ = gMapSelectArrowPosition.z;

        session.SpritePosition.x = coords.x;
        session.SpritePosition.y = coords.y;
        session.InteractionType = ViewportInteractionItem::None;

        PaintAddImageAsParent(session, imageId, { 0, 0, arrowZ }, { 32, 32, -1 }, { 0, 0, arrowZ + 18 });
    }

    if (screenMinY + 52 <= dpi->y)
        return;

    const TileElement* element = tile_element; // push tile_element

    uint16_t max_height = 0;
    do
    {
        max_height = std::max(max_height, static_cast<uint16_t>(element->GetClearanceZ()));
    } while (!(element++)->IsLastForTile());

    element--;

    if (element->GetType() == TileElementType::Surface && (element->AsSurface()->GetWaterHeight() > 0))
    {
        max_height = element->AsSurface()->GetWaterHeight();
    }

    if (partOfVirtualFloor)
    {
        // We must pretend this tile is at least as tall as the virtual floor
        max_height = std::max(max_height, VirtualFloorGetHeight());
    }

    if (screenMinY - (max_height + 32) >= dpi->y + dpi->height)
        return;

    session.SpritePosition.x = coords.x;
    session.SpritePosition.y = coords.y;
    session.Flags &= ~PaintSessionFlags::PassedSurface;

    int32_t previousBaseZ = 0;
    do
    {
        if (tile_element->IsInvisible())
        {
            continue;
        }

        // Only paint tile_elements below the clip height.
        if ((session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW) && (tile_element->GetBaseZ() > gClipHeight * COORDS_Z_STEP))
            continue;

        Direction direction = tile_element->GetDirectionWithOffset(rotation);
        int32_t baseZ = tile_element->GetBaseZ();

        // If we are on a new baseZ level, look through elements on the
        //  same baseZ and store any types might be relevant to others
        if (baseZ != previousBaseZ)
        {
            previousBaseZ = baseZ;
            session.PathElementOnSameHeight = nullptr;
            session.TrackElementOnSameHeight = nullptr;
            const TileElement* tile_element_sub_iterator = tile_element;
            while (!(tile_element_sub_iterator++)->IsLastForTile())
            {
                if (tile_element->IsInvisible())
                {
                    continue;
                }

                if (tile_element_sub_iterator->GetBaseZ() != tile_element->GetBaseZ())
                {
                    break;
                }
                auto type = tile_element_sub_iterator->GetType();
                if (type == TileElementType::Path)
                    session.PathElementOnSameHeight = tile_element_sub_iterator;
                else if (type == TileElementType::Track)
                    session.TrackElementOnSameHeight = tile_element_sub_iterator;
            }
        }

        CoordsXY mapPosition = session.MapPosition;
        session.CurrentlyDrawnTileElement = tile_element;
        // Setup the painting of for example: the underground, signs, rides, scenery, etc.
        switch (tile_element->GetType())
        {
            case TileElementType::Surface:
                PaintSurface(session, direction, baseZ, *(tile_element->AsSurface()));
                break;
            case TileElementType::Path:
                PaintPath(session, baseZ, *(tile_element->AsPath()));
                break;
            case TileElementType::Track:
                PaintTrack(session, direction, baseZ, *(tile_element->AsTrack()));
                break;
            case TileElementType::SmallScenery:
                PaintSmallScenery(session, direction, baseZ, *(tile_element->AsSmallScenery()));
                break;
            case TileElementType::Entrance:
                PaintEntrance(session, direction, baseZ, *(tile_element->AsEntrance()));
                break;
            case TileElementType::Wall:
                PaintWall(session, direction, baseZ, *(tile_element->AsWall()));
                break;
            case TileElementType::LargeScenery:
                PaintLargeScenery(session, direction, baseZ, *(tile_element->AsLargeScenery()));
                break;
            case TileElementType::Banner:
                PaintBanner(session, direction, baseZ, *(tile_element->AsBanner()));
                break;
        }
        session.MapPosition = mapPosition;
    } while (!(tile_element++)->IsLastForTile());

    if (gConfigGeneral.VirtualFloorStyle != VirtualFloorStyles::Off && partOfVirtualFloor)
    {
        VirtualFloorPaint(session);
    }

    if (!gShowSupportSegmentHeights)
    {
        return;
    }

    if ((tile_element - 1)->GetType() == TileElementType::Surface)
    {
        return;
    }

    static constexpr const int32_t segmentPositions[][3] = {
        { 0, 6, 2 },
        { 5, 4, 8 },
        { 1, 7, 3 },
    };

    for (std::size_t sy = 0; sy < std::size(segmentPositions); sy++)
    {
        for (std::size_t sx = 0; sx < std::size(segmentPositions[sy]); sx++)
        {
            uint16_t segmentHeight = session.SupportSegments[segmentPositions[sy][sx]].height;
            auto imageColourFlats = ImageId(SPR_LAND_TOOL_SIZE_1).WithTransparency(FilterPaletteID::PaletteDarken3);
            if (segmentHeight == 0xFFFF)
            {
                segmentHeight = session.Support.height;
                // white: 0b101101
                imageColourFlats = ImageId(SPR_LAND_TOOL_SIZE_1)
                                       .WithTransparency(FilterPaletteID::PaletteTranslucentBordeauxRedHighlight);
            }

            // Only draw supports below the clipping height.
            if ((session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW) && (segmentHeight > gClipHeight))
                continue;

            int32_t xOffset = static_cast<int32_t>(sy) * 10;
            int32_t yOffset = -22 + static_cast<int32_t>(sx) * 10;
            PaintStruct* ps = PaintAddImageAsParent(
                session, imageColourFlats, { xOffset, yOffset, segmentHeight }, { 10, 10, 1 },
                { xOffset + 1, yOffset + 16, segmentHeight });
            if (ps != nullptr)
            {
                ps->image_id = ps->image_id.WithTertiary(COLOUR_BORDEAUX_RED);
            }
        }
    }
}

void PaintUtilPushTunnelLeft(PaintSession& session, uint16_t height, uint8_t type)
{
    session.LeftTunnels[session.LeftTunnelCount] = { static_cast<uint8_t>((height / 16)), type };
    if (session.LeftTunnelCount < TUNNEL_MAX_COUNT - 1)
    {
        session.LeftTunnels[session.LeftTunnelCount + 1] = { 0xFF, 0xFF };
        session.LeftTunnelCount++;
    }
}

void PaintUtilPushTunnelRight(PaintSession& session, uint16_t height, uint8_t type)
{
    session.RightTunnels[session.RightTunnelCount] = { static_cast<uint8_t>((height / 16)), type };
    if (session.RightTunnelCount < TUNNEL_MAX_COUNT - 1)
    {
        session.RightTunnels[session.RightTunnelCount + 1] = { 0xFF, 0xFF };
        session.RightTunnelCount++;
    }
}

void PaintUtilSetVerticalTunnel(PaintSession& session, uint16_t height)
{
    session.VerticalTunnelHeight = height / 16;
}

void PaintUtilSetGeneralSupportHeight(PaintSession& session, int16_t height, uint8_t slope)
{
    if (session.Support.height >= height)
    {
        return;
    }

    PaintUtilForceSetGeneralSupportHeight(session, height, slope);
}

void PaintUtilForceSetGeneralSupportHeight(PaintSession& session, int16_t height, uint8_t slope)
{
    session.Support.height = height;
    session.Support.slope = slope;
}

const uint16_t segment_offsets[9] = {
    SEGMENT_B4, SEGMENT_B8, SEGMENT_BC, SEGMENT_C0, SEGMENT_C4, SEGMENT_C8, SEGMENT_CC, SEGMENT_D0, SEGMENT_D4,
};

void PaintUtilSetSegmentSupportHeight(PaintSession& session, int32_t segments, uint16_t height, uint8_t slope)
{
    SupportHeight* supportSegments = session.SupportSegments;
    for (std::size_t s = 0; s < std::size(segment_offsets); s++)
    {
        if (segments & segment_offsets[s])
        {
            supportSegments[s].height = height;
            if (height != 0xFFFF)
            {
                supportSegments[s].slope = slope;
            }
        }
    }
}

uint16_t PaintUtilRotateSegments(uint16_t segments, uint8_t rotation)
{
    uint8_t temp = segments & 0xFF;
    temp = Numerics::rol8(temp, rotation * 2);

    return (segments & 0xFF00) | temp;
}

bool PaintShouldShowHeightMarkers(const PaintSession& session, const uint32_t viewportFlag)
{
    auto dpi = &session.DPI;
    return (session.ViewFlags & viewportFlag) && (dpi->zoom_level <= ZoomLevel{ 0 });
}
