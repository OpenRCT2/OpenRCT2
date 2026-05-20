/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.TileElement.h"

#include "../../Game.h"
#include "../../Input.h"
#include "../../SpriteIds.h"
#include "../../config/Config.h"
#include "../../core/Numerics.hpp"
#include "../../interface/Viewport.h"
#include "../../profiling/Profiling.h"
#include "../../ride/RideData.h"
#include "../../ride/TrackData.h"
#include "../../ride/TrackPaint.h"
#include "../../world/Entrance.h"
#include "../../world/Footpath.h"
#include "../../world/Map.h"
#include "../../world/MapSelection.h"
#include "../../world/Scenery.h"
#include "../../world/tile_element/Slope.h"
#include "../../world/tile_element/SurfaceElement.h"
#include "../../world/tile_element/TileElement.h"
#include "../Paint.SessionFlags.h"
#include "../Paint.h"
#include "../VirtualFloor.h"
#include "Paint.Banner.h"
#include "Paint.Entrance.h"
#include "Paint.LargeScenery.h"
#include "Paint.Path.h"
#include "Paint.SmallScenery.h"
#include "Paint.Surface.h"
#include "Paint.Track.h"
#include "Paint.Wall.h"
#include "Segment.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

static void BlankTilesPaint(PaintSession& session, int32_t x, int32_t y);
static void PaintTileElementBase(PaintSession& session, const CoordsXY& origCoords);

/**
 *
 *  rct2: 0x0068B35F
 */
void TileElementPaintSetup(PaintSession& session, const CoordsXY& mapCoords, bool isTrackPiecePreview)
{
    PROFILED_FUNCTION();

    if (!MapIsEdge(mapCoords))
    {
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilForceSetGeneralSupportHeight(session, -1, 0);
        session.flags = isTrackPiecePreview ? PaintSessionFlags::IsTrackPiecePreview : 0;
        session.waterHeight = 0xFFFF;

        PaintTileElementBase(session, mapCoords);
    }
    else if (!(session.viewFlags & VIEWPORT_FLAG_TRANSPARENT_BACKGROUND))
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
    switch (session.currentRotation)
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

    if (bx <= session.rt.WorldY())
        return;
    dx -= 20;
    dx -= session.rt.WorldHeight();
    if (dx >= session.rt.WorldY())
        return;

    session.spritePosition.x = x;
    session.spritePosition.y = y;
    session.interactionType = ViewportInteractionItem::none;
    paintAddImageAsParent(session, ImageId(SPR_BLANK_TILE), { 0, 0, 16 }, { 32, 32, -1 });
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

    if ((session.viewFlags & VIEWPORT_FLAG_CLIP_VIEW))
    {
        if (coords.x < gClipSelectionA.x || coords.x > gClipSelectionB.x)
            return;
        if (coords.y < gClipSelectionA.y || coords.y > gClipSelectionB.y)
            return;
    }

    session.leftTunnels.clear();
    session.rightTunnels.clear();
    session.verticalTunnelHeight = 0xFF;
    session.mapPosition.x = coords.x;
    session.mapPosition.y = coords.y;

    auto* tile_element = MapGetFirstElementAt(session.mapPosition);
    if (tile_element == nullptr)
        return;
    uint8_t rotation = session.currentRotation;

    bool partOfVirtualFloor = false;

    if (Config::Get().general.virtualFloorStyle != VirtualFloorStyles::Off)
    {
        partOfVirtualFloor = VirtualFloorTileIsFloor(session.mapPosition);
    }

    switch (rotation)
    {
        case 0:
            break;
        case 1:
            coords.x += kCoordsXYStep;
            break;
        case 2:
            coords.x += kCoordsXYStep;
            coords.y += kCoordsXYStep;
            break;
        case 3:
            coords.y += kCoordsXYStep;
            break;
    }

    int32_t screenMinY = Translate3DTo2DWithZ(rotation, { coords, 0 }).y;

    // Display little yellow arrow when building footpaths?
    if (gMapSelectFlags.has(MapSelectFlag::enableArrow) && session.mapPosition.x == gMapSelectArrowPosition.x
        && session.mapPosition.y == gMapSelectArrowPosition.y)
    {
        uint8_t arrowRotation = (rotation + (gMapSelectArrowDirection & 3)) & 3;

        uint32_t imageIndex = arrowRotation + (gMapSelectArrowDirection & 0xFC) + PEEP_SPAWN_ARROW_0;
        ImageId imageId = ImageId(imageIndex, OpenRCT2::Drawing::Colour::yellow);
        int32_t arrowZ = gMapSelectArrowPosition.z;

        session.spritePosition.x = coords.x;
        session.spritePosition.y = coords.y;
        session.interactionType = ViewportInteractionItem::none;

        paintAddImageAsParent(session, imageId, { 0, 0, arrowZ }, { { 0, 0, arrowZ + 18 }, { 32, 32, -1 } });
    }

    if (screenMinY + 52 <= session.rt.WorldY())
        return;

    uint16_t maxHeight = 0;
    {
        const TileElement* element = tile_element;
        do
        {
            maxHeight = std::max(maxHeight, static_cast<uint16_t>(element->getClearanceZ()));
            if (element->getType() == TileElementType::Surface)
            {
                maxHeight = std::max(maxHeight, static_cast<uint16_t>(element->asSurface()->GetWaterHeight()));
            }
        } while (!(element++)->isLastForTile());
    }

    if (partOfVirtualFloor)
    {
        // We must pretend this tile is at least as tall as the virtual floor
        maxHeight = std::max(maxHeight, VirtualFloorGetHeight());
    }

    if (screenMinY - (maxHeight + 32) >= session.rt.WorldY() + session.rt.WorldHeight())
        return;

    session.spritePosition.x = coords.x;
    session.spritePosition.y = coords.y;
    session.flags &= ~PaintSessionFlags::PassedSurface;

    int32_t previousBaseZ = 0;
    do
    {
        if (tile_element->isInvisible())
        {
            continue;
        }

        // Only paint tile_elements below the clip height.
        if ((session.viewFlags & VIEWPORT_FLAG_CLIP_VIEW) && (tile_element->getBaseZ() > gClipHeight * kCoordsZStep))
        {
            // see-through off: don't paint this tile_element at all
            // see-through on: paint this tile_element as partial or hidden later on
            // note: surface elements are not painted even with see-through turned on
            if ((session.viewFlags & VIEWPORT_FLAG_CLIP_VIEW_SEE_THROUGH) == 0
                || tile_element->getType() == TileElementType::Surface)
            {
                continue;
            }
        }

        Direction direction = tile_element->getDirectionWithOffset(rotation);
        int32_t baseZ = tile_element->getBaseZ();

        // If we are on a new baseZ level, look through elements on the
        //  same baseZ and store any types might be relevant to others
        if (baseZ != previousBaseZ)
        {
            previousBaseZ = baseZ;
            session.pathElementOnSameHeight = nullptr;
            session.trackElementOnSameHeight = nullptr;
            const TileElement* tile_element_sub_iterator = tile_element;
            while (!(tile_element_sub_iterator++)->isLastForTile())
            {
                if (tile_element->isInvisible())
                {
                    continue;
                }

                if (tile_element_sub_iterator->getBaseZ() != tile_element->getBaseZ())
                {
                    break;
                }
                auto type = tile_element_sub_iterator->getType();
                if (type == TileElementType::Path)
                    session.pathElementOnSameHeight = tile_element_sub_iterator;
                else if (type == TileElementType::Track)
                    session.trackElementOnSameHeight = tile_element_sub_iterator;
            }
        }

        CoordsXY mapPosition = session.mapPosition;
        session.currentlyDrawnTileElement = tile_element;
        // Setup the painting of for example: the underground, signs, rides, scenery, etc.
        switch (tile_element->getType())
        {
            case TileElementType::Surface:
                PaintSurface(session, direction, baseZ, *(tile_element->asSurface()));
                break;
            case TileElementType::Path:
                PaintPath(session, baseZ, *(tile_element->asPath()));
                break;
            case TileElementType::Track:
                PaintTrack(session, direction, baseZ, *(tile_element->asTrack()));
                break;
            case TileElementType::SmallScenery:
                PaintSmallScenery(session, direction, baseZ, *(tile_element->asSmallScenery()));
                break;
            case TileElementType::Entrance:
                PaintEntrance(session, direction, baseZ, *(tile_element->asEntrance()));
                break;
            case TileElementType::Wall:
                PaintWall(session, direction, baseZ, *(tile_element->asWall()));
                break;
            case TileElementType::LargeScenery:
                PaintLargeScenery(session, direction, baseZ, *(tile_element->asLargeScenery()));
                break;
            case TileElementType::Banner:
                PaintBanner(session, direction, baseZ, *(tile_element->asBanner()));
                break;
        }
        session.mapPosition = mapPosition;
    } while (!(tile_element++)->isLastForTile());

    if (Config::Get().general.virtualFloorStyle != VirtualFloorStyles::Off && partOfVirtualFloor)
    {
        VirtualFloorPaint(session);
    }

    if (!gShowSupportSegmentHeights)
    {
        return;
    }

    if ((tile_element - 1)->getType() == TileElementType::Surface)
    {
        return;
    }

    static constexpr int32_t segmentPositions[][3] = {
        { 0, 6, 2 },
        { 5, 4, 8 },
        { 1, 7, 3 },
    };

    for (std::size_t sy = 0; sy < std::size(segmentPositions); sy++)
    {
        for (std::size_t sx = 0; sx < std::size(segmentPositions[sy]); sx++)
        {
            uint16_t segmentHeight = session.supportSegments[segmentPositions[sy][sx]].height;
            auto imageColourFlats = ImageId(SPR_LAND_TOOL_SIZE_1).WithTransparency(FilterPaletteID::paletteGlassBlack);
            if (segmentHeight == 0xFFFF)
            {
                segmentHeight = session.support.height;
                // white: 0b101101
                imageColourFlats = ImageId(SPR_LAND_TOOL_SIZE_1)
                                       .WithTransparency(FilterPaletteID::paletteTranslucentBordeauxRedHighlight);
            }

            // Only draw supports below the clipping height.
            if ((session.viewFlags & VIEWPORT_FLAG_CLIP_VIEW) && (segmentHeight > gClipHeight))
                continue;

            int32_t xOffset = static_cast<int32_t>(sy) * 10;
            int32_t yOffset = -22 + static_cast<int32_t>(sx) * 10;
            paintAddImageAsParent(
                session, imageColourFlats, { xOffset, yOffset, segmentHeight },
                { { xOffset + 1, yOffset + 16, segmentHeight }, { 10, 10, 1 } });
        }
    }
}

void PaintUtilSetGeneralSupportHeight(PaintSession& session, int16_t height)
{
    if (session.support.height >= height)
    {
        return;
    }

    PaintUtilForceSetGeneralSupportHeight(session, height, kTileSlopeAboveTrackOrScenery);
}

void PaintUtilForceSetGeneralSupportHeight(PaintSession& session, int16_t height, uint8_t slope)
{
    session.support.height = height;
    session.support.slope = slope;
}

const uint16_t kSegmentOffsets[9] = {
    EnumToFlag(PaintSegment::top),      EnumToFlag(PaintSegment::left),       EnumToFlag(PaintSegment::right),
    EnumToFlag(PaintSegment::bottom),   EnumToFlag(PaintSegment::centre),     EnumToFlag(PaintSegment::topLeft),
    EnumToFlag(PaintSegment::topRight), EnumToFlag(PaintSegment::bottomLeft), EnumToFlag(PaintSegment::bottomRight),
};

void PaintUtilSetSegmentSupportHeight(PaintSession& session, int32_t segments, uint16_t height, uint8_t slope)
{
    SupportHeight* supportSegments = session.supportSegments;
    for (std::size_t s = 0; s < std::size(kSegmentOffsets); s++)
    {
        if (segments & kSegmentOffsets[s])
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
    // Only the value representing PaintSegment::centre falls beyond 0xFF, so this will be kept in place.
    uint8_t temp = segments & 0xFF;
    temp = Numerics::rol8(temp, rotation * 2);

    return (segments & 0xFF00) | temp;
}

bool PaintShouldShowHeightMarkers(const PaintSession& session, const uint32_t viewportFlag)
{
    auto rt = &session.rt;
    return (session.viewFlags & viewportFlag) && (rt->zoom_level <= ZoomLevel{ 0 });
}
