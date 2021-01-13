/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../ride/RideData.h"
#include "../../ride/TrackData.h"
#include "../../ride/TrackPaint.h"
#include "../../sprites.h"
#include "../../world/Banner.h"
#include "../../world/Entrance.h"
#include "../../world/Footpath.h"
#include "../../world/Scenery.h"
#include "../../world/Sprite.h"
#include "../../world/Surface.h"
#include "../Paint.h"
#include "../Supports.h"
#include "../VirtualFloor.h"
#include "Paint.Surface.h"

#include <algorithm>

#ifdef __TESTPAINT__
uint16_t testPaintVerticalTunnelHeight;
#endif

static void blank_tiles_paint(paint_session* session, int32_t x, int32_t y);
static void sub_68B3FB(paint_session* session, int32_t x, int32_t y);

const int32_t SEGMENTS_ALL = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC
    | SEGMENT_D0 | SEGMENT_D4;

/**
 *
 *  rct2: 0x0068B35F
 */
void tile_element_paint_setup(paint_session* session, int32_t x, int32_t y)
{
    if (x < gMapSizeUnits && y < gMapSizeUnits && x >= 32 && y >= 32)
    {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_force_set_general_support_height(session, -1, 0);
        session->Unk141E9DB = 0;
        session->WaterHeight = 0xFFFF;

        sub_68B3FB(session, x, y);
    }
    else if (!(session->ViewFlags & VIEWPORT_FLAG_TRANSPARENT_BACKGROUND))
    {
        blank_tiles_paint(session, x, y);
    }
}

/**
 *
 *  rct2: 0x0068B2B7
 */
void sub_68B2B7(paint_session* session, const CoordsXY& mapCoords)
{
    if (mapCoords.x < gMapSizeUnits && mapCoords.y < gMapSizeUnits && mapCoords.x >= 32 && mapCoords.y >= 32)
    {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_force_set_general_support_height(session, -1, 0);
        session->WaterHeight = 0xFFFF;
        session->Unk141E9DB = G141E9DB_FLAG_2;

        sub_68B3FB(session, mapCoords.x, mapCoords.y);
    }
    else if (!(session->ViewFlags & VIEWPORT_FLAG_TRANSPARENT_BACKGROUND))
    {
        blank_tiles_paint(session, mapCoords.x, mapCoords.y);
    }
}

/**
 *
 *  rct2: 0x0068B60E
 */
static void blank_tiles_paint(paint_session* session, int32_t x, int32_t y)
{
    int32_t dx = 0;
    switch (session->CurrentRotation)
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

    rct_drawpixelinfo* dpi = &session->DPI;
    if (bx <= dpi->y)
        return;
    dx -= 20;
    dx -= dpi->height;
    if (dx >= dpi->y)
        return;

    session->SpritePosition.x = x;
    session->SpritePosition.y = y;
    session->InteractionType = ViewportInteractionItem::None;
    PaintAddImageAsParent(session, SPR_BLANK_TILE, 0, 0, 32, 32, -1, 16);
}

bool gShowSupportSegmentHeights = false;

/**
 *
 *  rct2: 0x0068B3FB
 */
static void sub_68B3FB(paint_session* session, int32_t x, int32_t y)
{
    rct_drawpixelinfo* dpi = &session->DPI;

    if ((session->ViewFlags & VIEWPORT_FLAG_CLIP_VIEW))
    {
        if (x < gClipSelectionA.x || x > gClipSelectionB.x)
            return;
        if (y < gClipSelectionA.y || y > gClipSelectionB.y)
            return;
    }

    session->LeftTunnelCount = 0;
    session->RightTunnelCount = 0;
    session->LeftTunnels[0] = { 0xFF, 0xFF };
    session->RightTunnels[0] = { 0xFF, 0xFF };
    session->VerticalTunnelHeight = 0xFF;
    session->MapPosition.x = x;
    session->MapPosition.y = y;

    TileElement* tile_element = map_get_first_element_at(session->MapPosition);
    if (tile_element == nullptr)
        return;
    uint8_t rotation = session->CurrentRotation;

    bool partOfVirtualFloor = false;
#ifndef __TESTPAINT__
    if (gConfigGeneral.virtual_floor_style != VirtualFloorStyles::Off)
    {
        partOfVirtualFloor = virtual_floor_tile_is_floor(session->MapPosition);
    }
#endif // __TESTPAINT__

    int32_t dx = 0;
    switch (rotation)
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
    dx >>= 1;
    // Display little yellow arrow when building footpaths?
    if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_ARROW) && session->MapPosition.x == gMapSelectArrowPosition.x
        && session->MapPosition.y == gMapSelectArrowPosition.y)
    {
        uint8_t arrowRotation = (rotation + (gMapSelectArrowDirection & 3)) & 3;

        uint32_t imageId = arrowRotation + (gMapSelectArrowDirection & 0xFC) + 0x20900C27;
        int32_t arrowZ = gMapSelectArrowPosition.z;

        session->SpritePosition.x = x;
        session->SpritePosition.y = y;
        session->InteractionType = ViewportInteractionItem::None;

        PaintAddImageAsParent(session, imageId, 0, 0, 32, 32, -1, arrowZ, 0, 0, arrowZ + 18);
    }
    int32_t bx = dx + 52;

    if (bx <= dpi->y)
        return;

    const TileElement* element = tile_element; // push tile_element

    uint16_t max_height = 0;
    do
    {
        max_height = std::max(max_height, static_cast<uint16_t>(element->GetClearanceZ()));
    } while (!(element++)->IsLastForTile());

    element--;

    if (element->GetType() == TILE_ELEMENT_TYPE_SURFACE && (element->AsSurface()->GetWaterHeight() > 0))
    {
        max_height = element->AsSurface()->GetWaterHeight();
    }

#ifndef __TESTPAINT__
    if (partOfVirtualFloor)
    {
        // We must pretend this tile is at least as tall as the virtual floor
        max_height = std::max(max_height, virtual_floor_get_height());
    }
#endif // __TESTPAINT__

    dx -= max_height + 32;

    element = tile_element; // pop tile_element
    dx -= dpi->height;
    if (dx >= dpi->y)
        return;

    session->SpritePosition.x = x;
    session->SpritePosition.y = y;
    session->DidPassSurface = false;
    int32_t previousBaseZ = 0;
    do
    {
        if (tile_element->IsInvisible())
        {
            continue;
        }

        // Only paint tile_elements below the clip height.
        if ((session->ViewFlags & VIEWPORT_FLAG_CLIP_VIEW) && (tile_element->GetBaseZ() > gClipHeight * COORDS_Z_STEP))
            continue;

        Direction direction = tile_element->GetDirectionWithOffset(rotation);
        int32_t baseZ = tile_element->GetBaseZ();

        // If we are on a new baseZ level, look through elements on the
        //  same baseZ and store any types might be relevant to others
        if (baseZ != previousBaseZ)
        {
            previousBaseZ = baseZ;
            session->PathElementOnSameHeight = nullptr;
            session->TrackElementOnSameHeight = nullptr;
            TileElement* tile_element_sub_iterator = tile_element;
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
                switch (tile_element_sub_iterator->GetType())
                {
                    case TILE_ELEMENT_TYPE_PATH:
                        session->PathElementOnSameHeight = tile_element_sub_iterator;
                        break;
                    case TILE_ELEMENT_TYPE_TRACK:
                        session->TrackElementOnSameHeight = tile_element_sub_iterator;
                        break;
                }
            }
        }

        CoordsXY mapPosition = session->MapPosition;
        session->CurrentlyDrawnItem = tile_element;
        // Setup the painting of for example: the underground, signs, rides, scenery, etc.
        switch (tile_element->GetType())
        {
            case TILE_ELEMENT_TYPE_SURFACE:
                surface_paint(session, direction, baseZ, tile_element);
                break;
            case TILE_ELEMENT_TYPE_PATH:
                path_paint(session, baseZ, tile_element);
                break;
            case TILE_ELEMENT_TYPE_TRACK:
                track_paint(session, direction, baseZ, tile_element);
                break;
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                scenery_paint(session, direction, baseZ, tile_element);
                break;
            case TILE_ELEMENT_TYPE_ENTRANCE:
                entrance_paint(session, direction, baseZ, tile_element);
                break;
            case TILE_ELEMENT_TYPE_WALL:
                fence_paint(session, direction, baseZ, tile_element);
                break;
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                large_scenery_paint(session, direction, baseZ, tile_element);
                break;
            case TILE_ELEMENT_TYPE_BANNER:
                banner_paint(session, direction, baseZ, tile_element);
                break;
        }
        session->MapPosition = mapPosition;
    } while (!(tile_element++)->IsLastForTile());

#ifndef __TESTPAINT__
    if (gConfigGeneral.virtual_floor_style != VirtualFloorStyles::Off && partOfVirtualFloor)
    {
        virtual_floor_paint(session);
    }
#endif // __TESTPAINT__

    if (!gShowSupportSegmentHeights)
    {
        return;
    }

    if ((tile_element - 1)->GetType() == TILE_ELEMENT_TYPE_SURFACE)
    {
        return;
    }

    static constexpr const int32_t segmentPositions[][3] = {
        { 0, 6, 2 },
        { 5, 4, 8 },
        { 1, 7, 3 },
    };

    for (int32_t sy = 0; sy < 3; sy++)
    {
        for (int32_t sx = 0; sx < 3; sx++)
        {
            uint16_t segmentHeight = session->SupportSegments[segmentPositions[sy][sx]].height;
            int32_t imageColourFlats = 0b101111 << 19 | IMAGE_TYPE_TRANSPARENT;
            if (segmentHeight == 0xFFFF)
            {
                segmentHeight = session->Support.height;
                // white: 0b101101
                imageColourFlats = 0b111011 << 19 | IMAGE_TYPE_TRANSPARENT;
            }

            // Only draw supports below the clipping height.
            if ((session->ViewFlags & VIEWPORT_FLAG_CLIP_VIEW) && (segmentHeight > gClipHeight))
                continue;

            int32_t xOffset = sy * 10;
            int32_t yOffset = -22 + sx * 10;
            paint_struct* ps = PaintAddImageAsParent(
                session, 5504 | imageColourFlats, xOffset, yOffset, 10, 10, 1, segmentHeight, xOffset + 1, yOffset + 16,
                segmentHeight);
            if (ps != nullptr)
            {
                ps->flags &= PAINT_STRUCT_FLAG_IS_MASKED;
                ps->colour_image_id = COLOUR_BORDEAUX_RED;
            }
        }
    }
}

void paint_util_push_tunnel_left(paint_session* session, uint16_t height, uint8_t type)
{
    session->LeftTunnels[session->LeftTunnelCount] = { static_cast<uint8_t>((height / 16)), type };
    if (session->LeftTunnelCount < TUNNEL_MAX_COUNT - 1)
    {
        session->LeftTunnels[session->LeftTunnelCount + 1] = { 0xFF, 0xFF };
        session->LeftTunnelCount++;
    }
}

void paint_util_push_tunnel_right(paint_session* session, uint16_t height, uint8_t type)
{
    session->RightTunnels[session->RightTunnelCount] = { static_cast<uint8_t>((height / 16)), type };
    if (session->RightTunnelCount < TUNNEL_MAX_COUNT - 1)
    {
        session->RightTunnels[session->RightTunnelCount + 1] = { 0xFF, 0xFF };
        session->RightTunnelCount++;
    }
}

void paint_util_set_vertical_tunnel(paint_session* session, uint16_t height)
{
#ifdef __TESTPAINT__
    testPaintVerticalTunnelHeight = height;
#endif
    session->VerticalTunnelHeight = height / 16;
}

void paint_util_set_general_support_height(paint_session* session, int16_t height, uint8_t slope)
{
    if (session->Support.height >= height)
    {
        return;
    }

    paint_util_force_set_general_support_height(session, height, slope);
}

void paint_util_force_set_general_support_height(paint_session* session, int16_t height, uint8_t slope)
{
    session->Support.height = height;
    session->Support.slope = slope;
}

const uint16_t segment_offsets[9] = { SEGMENT_B4, SEGMENT_B8, SEGMENT_BC, SEGMENT_C0, SEGMENT_C4,
                                      SEGMENT_C8, SEGMENT_CC, SEGMENT_D0, SEGMENT_D4 };

void paint_util_set_segment_support_height(paint_session* session, int32_t segments, uint16_t height, uint8_t slope)
{
    support_height* supportSegments = session->SupportSegments;
    for (int32_t s = 0; s < 9; s++)
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

uint16_t paint_util_rotate_segments(uint16_t segments, uint8_t rotation)
{
    uint8_t temp = segments & 0xFF;
    temp = rol8(temp, rotation * 2);

    return (segments & 0xFF00) | temp;
}

bool PaintShouldShowHeightMarkers(const paint_session* session, const uint32_t viewportFlag)
{
    auto dpi = &session->DPI;
    return (session->ViewFlags & viewportFlag) && (dpi->zoom_level <= 0);
}
