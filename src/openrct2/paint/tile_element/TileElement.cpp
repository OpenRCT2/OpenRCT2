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

#include "../../config/Config.h"
#include "../../core/Math.hpp"
#include "../../drawing/Drawing.h"
#include "../../Input.h"
#include "../../Game.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../ride/RideData.h"
#include "../../ride/TrackData.h"
#include "../../ride/TrackPaint.h"
#include "../../world/Sprite.h"
#include "../../world/Banner.h"
#include "../../world/Entrance.h"
#include "../../world/Footpath.h"
#include "../../world/Scenery.h"
#include "../../sprites.h"
#include "../Paint.h"
#include "../Supports.h"
#include "../VirtualFloor.h"
#include "Surface.h"
#include "TileElement.h"

#ifdef __TESTPAINT__
uint16 testPaintVerticalTunnelHeight;
#endif

static void blank_tiles_paint(paint_session * session, sint32 x, sint32 y);
static void sub_68B3FB(paint_session * session, sint32 x, sint32 y);

const sint32 SEGMENTS_ALL = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4;

/**
 *
 *  rct2: 0x0068B35F
 */
void tile_element_paint_setup(paint_session * session, sint32 x, sint32 y)
{
    if (
        x < gMapSizeUnits &&
        y < gMapSizeUnits &&
        x >= 32 &&
        y >= 32
    ) {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_force_set_general_support_height(session, -1, 0);
        session->Unk141E9DB = 0;
        session->WaterHeight = 0xFFFF;

        sub_68B3FB(session, x, y);
    } else {
        blank_tiles_paint(session, x, y);
    }
}

/**
 *
 *  rct2: 0x0068B2B7
 */
void sub_68B2B7(paint_session * session, sint32 x, sint32 y)
{
    if (
        x < gMapSizeUnits &&
        y < gMapSizeUnits &&
        x >= 32 &&
        y >= 32
    ) {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_force_set_general_support_height(session, -1, 0);
        session->WaterHeight = 0xFFFF;
        session->Unk141E9DB = G141E9DB_FLAG_2;

        sub_68B3FB(session, x, y);
    } else {
        blank_tiles_paint(session, x, y);
    }
}

/**
 *
 *  rct2: 0x0068B60E
 */
static void blank_tiles_paint(paint_session * session, sint32 x, sint32 y)
{
    sint32 dx = 0;
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
    sint32 bx = dx + 32;

    rct_drawpixelinfo * dpi = session->Unk140E9A8;
    if (bx <= dpi->y) return;
    dx -= 20;
    dx -= dpi->height;
    if (dx >= dpi->y) return;

    session->SpritePosition.x = x;
    session->SpritePosition.y = y;
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
    sub_98196C(session, 3123, 0, 0, 32, 32, -1, 16);
}

bool gShowSupportSegmentHeights = false;

/**
 *
 *  rct2: 0x0068B3FB
 */
static void sub_68B3FB(paint_session * session, sint32 x, sint32 y)
{
    rct_drawpixelinfo *dpi = session->Unk140E9A8;

    session->LeftTunnelCount = 0;
    session->RightTunnelCount = 0;
    session->LeftTunnels[0] = {0xFF, 0xFF};
    session->RightTunnels[0] = {0xFF, 0xFF};
    session->VerticalTunnelHeight = 0xFF;
    session->MapPosition.x = x;
    session->MapPosition.y = y;

    rct_tile_element* tile_element = map_get_first_element_at(x >> 5, y >> 5);
    uint8             rotation     = session->CurrentRotation;

    bool partOfVirtualFloor = false;
#ifndef __TESTPAINT__
    if (gConfigGeneral.use_virtual_floor)
    {
        partOfVirtualFloor = virtual_floor_tile_is_floor(session->MapPosition.x, session->MapPosition.y);
    }
#endif // __TESTPAINT__

    sint32 dx = 0;
    switch (rotation) {
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
    if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_ARROW) &&
        session->MapPosition.x == gMapSelectArrowPosition.x &&
        session->MapPosition.y == gMapSelectArrowPosition.y
    ) {
        uint8 arrowRotation =
            (rotation
            + (gMapSelectArrowDirection & 3)) & 3;

        uint32 imageId =
            arrowRotation +
            (gMapSelectArrowDirection & 0xFC) +
            0x20900C27;
        sint32 arrowZ = gMapSelectArrowPosition.z;

        session->SpritePosition.x = x;
        session->SpritePosition.y = y;
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;

        sub_98197C(session, imageId, 0, 0, 32, 32, -1, arrowZ, 0, 0, arrowZ + 18);
    }
    sint32 bx = dx + 52;

    if (bx <= dpi->y)
        return;

    const rct_tile_element* element = tile_element;//push tile_element

    uint16 max_height = 0;
    do{
        max_height = Math::Max(max_height, (uint16)element->clearance_height);
    } while (!tile_element_is_last_for_tile(element++));

    element--;

    if (tile_element_get_type(element) == TILE_ELEMENT_TYPE_SURFACE &&
        (map_get_water_height(element) > 0))
    {
        max_height = map_get_water_height(element) * 2;
    }

    max_height *= 8;

#ifndef __TESTPAINT__
    if (partOfVirtualFloor)
    {
        // We must pretend this tile is at least as tall as the virtual floor
        max_height = std::max(max_height, virtual_floor_get_height());
    }
#endif // __TESTPAINT__

    dx -= max_height + 32;

    element = tile_element;//pop tile_element
    dx -= dpi->height;
    if (dx >= dpi->y)
        return;

    session->SpritePosition.x = x;
    session->SpritePosition.y = y;
    session->DidPassSurface = false;
    sint32 previousHeight = 0;
    do {
        // Only paint tile_elements below the clip height.
        if ((gCurrentViewportFlags & VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT) && (tile_element->base_height > gClipHeight))
            continue;

        sint32 direction = tile_element_get_direction_with_offset(tile_element, rotation);
        sint32 height = tile_element->base_height * 8;

        // If we are on a new height level, look through elements on the
        //  same height and store any types might be relevant to others
        if (height != previousHeight)
        {
            previousHeight = height;
            session->PathElementOnSameHeight = nullptr;
            session->TrackElementOnSameHeight = nullptr;
            rct_tile_element * tile_element_sub_iterator = tile_element;
            while (!tile_element_is_last_for_tile(tile_element_sub_iterator++))
            {
                if (tile_element_sub_iterator->base_height != tile_element->base_height)
                {
                    break;
                }
                switch (tile_element_get_type(tile_element_sub_iterator))
                {
                case TILE_ELEMENT_TYPE_PATH:
                    session->PathElementOnSameHeight = tile_element_sub_iterator;
                    break;
                case TILE_ELEMENT_TYPE_TRACK:
                    session->TrackElementOnSameHeight = tile_element_sub_iterator;
                    break;
                case TILE_ELEMENT_TYPE_CORRUPT:
                    // To preserve regular behaviour, make an element hidden by
                    //  corruption also invisible to this method.
                    if (tile_element_is_last_for_tile(tile_element))
                    {
                        break;
                    }
                    tile_element_sub_iterator++;
                    break;
                }
            }
        }

        LocationXY16 dword_9DE574 = session->MapPosition;
        session->CurrentlyDrawnItem = tile_element;
        // Setup the painting of for example: the underground, signs, rides, scenery, etc.
        switch (tile_element_get_type(tile_element))
        {
        case TILE_ELEMENT_TYPE_SURFACE:
            surface_paint(session, direction, height, tile_element);
            break;
        case TILE_ELEMENT_TYPE_PATH:
            path_paint(session, direction, height, tile_element);
            break;
        case TILE_ELEMENT_TYPE_TRACK:
            track_paint(session, direction, height, tile_element);
            break;
        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            scenery_paint(session, direction, height, tile_element);
            break;
        case TILE_ELEMENT_TYPE_ENTRANCE:
            entrance_paint(session, direction, height, tile_element);
            break;
        case TILE_ELEMENT_TYPE_WALL:
            fence_paint(session, direction, height, tile_element);
            break;
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            large_scenery_paint(session, direction, height, tile_element);
            break;
        case TILE_ELEMENT_TYPE_BANNER:
            banner_paint(session, direction, height, tile_element);
            break;
        // A corrupt element inserted by OpenRCT2 itself, which skips the drawing of the next element only.
        case TILE_ELEMENT_TYPE_CORRUPT:
            if (tile_element_is_last_for_tile(tile_element))
                return;
            tile_element++;
            break;
        default:
            // An undefined map element is most likely a corrupt element inserted by 8 cars' MOM feature to skip drawing of all elements after it.
            return;
        }
        session->MapPosition = dword_9DE574;
    } while (!tile_element_is_last_for_tile(tile_element++));

#ifndef __TESTPAINT__
    if (gConfigGeneral.use_virtual_floor && partOfVirtualFloor)
    {
        virtual_floor_paint(session);
    }
#endif // __TESTPAINT__

    if (!gShowSupportSegmentHeights) {
        return;
    }

    if (tile_element_get_type(tile_element - 1) == TILE_ELEMENT_TYPE_SURFACE) {
        return;
    }

    static constexpr const sint32 segmentPositions[][3] = {
        {0, 6, 2},
        {5, 4, 8},
        {1, 7, 3},
    };

    for (sint32 sy = 0; sy < 3; sy++) {
        for (sint32 sx = 0; sx < 3; sx++) {
            uint16 segmentHeight = session->SupportSegments[segmentPositions[sy][sx]].height;
            sint32 imageColourFlats = 0b101111 << 19 | IMAGE_TYPE_TRANSPARENT;
            if (segmentHeight == 0xFFFF) {
                segmentHeight = session->Support.height;
                // white: 0b101101
                imageColourFlats = 0b111011 << 19 | IMAGE_TYPE_TRANSPARENT;
            }

            // Only draw supports below the clipping height.
            if ((gCurrentViewportFlags & VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT) && (segmentHeight > gClipHeight)) continue;

            sint32 xOffset = sy * 10;
            sint32 yOffset = -22 + sx * 10;
            paint_struct * ps      = sub_98197C(
                session, 5504 | imageColourFlats, xOffset, yOffset, 10, 10, 1, segmentHeight, xOffset + 1, yOffset + 16,
                segmentHeight);
            if (ps != nullptr) {
                ps->flags &= PAINT_STRUCT_FLAG_IS_MASKED;
                ps->colour_image_id = COLOUR_BORDEAUX_RED;
            }

        }
    }
}

void paint_util_push_tunnel_left(paint_session * session, uint16 height, uint8 type)
{
    session->LeftTunnels[session->LeftTunnelCount] = {static_cast<uint8>((height / 16)), type};
    if (session->LeftTunnelCount < TUNNEL_MAX_COUNT - 1) {
        session->LeftTunnels[session->LeftTunnelCount + 1] = {0xFF, 0xFF};
        session->LeftTunnelCount++;
    }
}

void paint_util_push_tunnel_right(paint_session * session, uint16 height, uint8 type)
{
    session->RightTunnels[session->RightTunnelCount] = {static_cast<uint8>((height / 16)), type};
    if (session->RightTunnelCount < TUNNEL_MAX_COUNT - 1) {
        session->RightTunnels[session->RightTunnelCount + 1] = {0xFF, 0xFF};
        session->RightTunnelCount++;
    }
}

void paint_util_set_vertical_tunnel(paint_session * session, uint16 height)
{
#ifdef __TESTPAINT__
    testPaintVerticalTunnelHeight = height;
#endif
    session->VerticalTunnelHeight = height / 16;
}

void paint_util_set_general_support_height(paint_session * session, sint16 height, uint8 slope)
{
    if (session->Support.height >= height) {
        return;
    }

    paint_util_force_set_general_support_height(session, height, slope);
}

void paint_util_force_set_general_support_height(paint_session * session, sint16 height, uint8 slope)
{
    session->Support.height = height;
    session->Support.slope = slope;
}

const uint16 segment_offsets[9] = {
    SEGMENT_B4,
    SEGMENT_B8,
    SEGMENT_BC,
    SEGMENT_C0,
    SEGMENT_C4,
    SEGMENT_C8,
    SEGMENT_CC,
    SEGMENT_D0,
    SEGMENT_D4
};

void paint_util_set_segment_support_height(paint_session * session, sint32 segments, uint16 height, uint8 slope)
{
    support_height * supportSegments = session->SupportSegments;
    for (sint32 s = 0; s < 9; s++) {
        if (segments & segment_offsets[s]) {
            supportSegments[s].height = height;
            if (height != 0xFFFF) {
                supportSegments[s].slope = slope;
            }
        }
    }
}

uint16 paint_util_rotate_segments(uint16 segments, uint8 rotation)
{
    uint8 temp = segments & 0xFF;
    temp = rol8(temp, rotation * 2);

    return (segments & 0xFF00) | temp;
}
