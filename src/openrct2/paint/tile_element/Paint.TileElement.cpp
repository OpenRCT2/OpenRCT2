/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
#include "../../world/Surface.h"
#include "../../sprites.h"
#include "../Paint.h"
#include "../Supports.h"
#include "../VirtualFloor.h"
#include "Paint.Surface.h"
#include "Paint.TileElement.h"

#ifdef __TESTPAINT__
uint16_t testPaintVerticalTunnelHeight;
#endif

static void blank_tiles_paint(paint_session * session, int32_t x, int32_t y);
static void sub_68B3FB(paint_session * session, int32_t x, int32_t y);

const int32_t SEGMENTS_ALL = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4;

/**
 *
 *  rct2: 0x0068B35F
 */
void tile_element_paint_setup(paint_session * session, int32_t x, int32_t y)
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
void sub_68B2B7(paint_session * session, int32_t x, int32_t y)
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
static void blank_tiles_paint(paint_session * session, int32_t x, int32_t y)
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

    rct_drawpixelinfo * dpi = session->DPI;
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
static void sub_68B3FB(paint_session * session, int32_t x, int32_t y)
{
    rct_drawpixelinfo *dpi = session->DPI;

    if ((gCurrentViewportFlags & VIEWPORT_FLAG_CLIP_VIEW))
    {
        if (x / 32 < gClipSelectionA.x || x / 32 > gClipSelectionB.x)
            return;
        if (y / 32 < gClipSelectionA.y || y / 32 > gClipSelectionB.y)
            return;
    }

    session->LeftTunnelCount = 0;
    session->RightTunnelCount = 0;
    session->LeftTunnels[0] = {0xFF, 0xFF};
    session->RightTunnels[0] = {0xFF, 0xFF};
    session->VerticalTunnelHeight = 0xFF;
    session->MapPosition.x = x;
    session->MapPosition.y = y;

    rct_tile_element* tile_element = map_get_first_element_at(x >> 5, y >> 5);
    uint8_t             rotation     = session->CurrentRotation;

    bool partOfVirtualFloor = false;
#ifndef __TESTPAINT__
    if (gConfigGeneral.virtual_floor_style != VIRTUAL_FLOOR_STYLE_OFF)
    {
        partOfVirtualFloor = virtual_floor_tile_is_floor(session->MapPosition.x, session->MapPosition.y);
    }
#endif // __TESTPAINT__

    int32_t dx = 0;
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
        uint8_t arrowRotation =
            (rotation
            + (gMapSelectArrowDirection & 3)) & 3;

        uint32_t imageId =
            arrowRotation +
            (gMapSelectArrowDirection & 0xFC) +
            0x20900C27;
        int32_t arrowZ = gMapSelectArrowPosition.z;

        session->SpritePosition.x = x;
        session->SpritePosition.y = y;
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;

        sub_98197C(session, imageId, 0, 0, 32, 32, -1, arrowZ, 0, 0, arrowZ + 18);
    }
    int32_t bx = dx + 52;

    if (bx <= dpi->y)
        return;

    const rct_tile_element* element = tile_element;//push tile_element

    uint16_t max_height = 0;
    do{
        max_height = std::max(max_height, (uint16_t)element->clearance_height);
    } while (!(element++)->IsLastForTile());

    element--;

    if (element->GetType() == TILE_ELEMENT_TYPE_SURFACE &&
        (surface_get_water_height(element) > 0))
    {
        max_height = surface_get_water_height(element) * 2;
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
    int32_t previousHeight = 0;
    do {
        // Only paint tile_elements below the clip height.
        if ((gCurrentViewportFlags & VIEWPORT_FLAG_CLIP_VIEW) && (tile_element->base_height > gClipHeight))
            continue;

        int32_t direction = tile_element_get_direction_with_offset(tile_element, rotation);
        int32_t height = tile_element->base_height * 8;

        // If we are on a new height level, look through elements on the
        //  same height and store any types might be relevant to others
        if (height != previousHeight)
        {
            previousHeight = height;
            session->PathElementOnSameHeight = nullptr;
            session->TrackElementOnSameHeight = nullptr;
            rct_tile_element * tile_element_sub_iterator = tile_element;
            while (!(tile_element_sub_iterator++)->IsLastForTile())
            {
                if (tile_element_sub_iterator->base_height != tile_element->base_height)
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
                case TILE_ELEMENT_TYPE_CORRUPT:
                    // To preserve regular behaviour, make an element hidden by
                    //  corruption also invisible to this method.
                    if (tile_element->IsLastForTile())
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
        switch (tile_element->GetType())
        {
        case TILE_ELEMENT_TYPE_SURFACE:
            surface_paint(session, direction, height, tile_element);
            break;
        case TILE_ELEMENT_TYPE_PATH:
            path_paint(session, height, tile_element);
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
            if (tile_element->IsLastForTile())
                return;
            tile_element++;
            break;
        default:
            // An undefined map element is most likely a corrupt element inserted by 8 cars' MOM feature to skip drawing of all elements after it.
            return;
        }
        session->MapPosition = dword_9DE574;
    } while (!(tile_element++)->IsLastForTile());

#ifndef __TESTPAINT__
    if (gConfigGeneral.virtual_floor_style != VIRTUAL_FLOOR_STYLE_OFF && partOfVirtualFloor)
    {
        virtual_floor_paint(session);
    }
#endif // __TESTPAINT__

    if (!gShowSupportSegmentHeights) {
        return;
    }

    if ((tile_element - 1)->GetType() == TILE_ELEMENT_TYPE_SURFACE) {
        return;
    }

    static constexpr const int32_t segmentPositions[][3] = {
        {0, 6, 2},
        {5, 4, 8},
        {1, 7, 3},
    };

    for (int32_t sy = 0; sy < 3; sy++) {
        for (int32_t sx = 0; sx < 3; sx++) {
            uint16_t segmentHeight = session->SupportSegments[segmentPositions[sy][sx]].height;
            int32_t imageColourFlats = 0b101111 << 19 | IMAGE_TYPE_TRANSPARENT;
            if (segmentHeight == 0xFFFF) {
                segmentHeight = session->Support.height;
                // white: 0b101101
                imageColourFlats = 0b111011 << 19 | IMAGE_TYPE_TRANSPARENT;
            }

            // Only draw supports below the clipping height.
            if ((gCurrentViewportFlags & VIEWPORT_FLAG_CLIP_VIEW) && (segmentHeight > gClipHeight)) continue;

            int32_t xOffset = sy * 10;
            int32_t yOffset = -22 + sx * 10;
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

void paint_util_push_tunnel_left(paint_session * session, uint16_t height, uint8_t type)
{
    session->LeftTunnels[session->LeftTunnelCount] = {static_cast<uint8_t>((height / 16)), type};
    if (session->LeftTunnelCount < TUNNEL_MAX_COUNT - 1) {
        session->LeftTunnels[session->LeftTunnelCount + 1] = {0xFF, 0xFF};
        session->LeftTunnelCount++;
    }
}

void paint_util_push_tunnel_right(paint_session * session, uint16_t height, uint8_t type)
{
    session->RightTunnels[session->RightTunnelCount] = {static_cast<uint8_t>((height / 16)), type};
    if (session->RightTunnelCount < TUNNEL_MAX_COUNT - 1) {
        session->RightTunnels[session->RightTunnelCount + 1] = {0xFF, 0xFF};
        session->RightTunnelCount++;
    }
}

void paint_util_set_vertical_tunnel(paint_session * session, uint16_t height)
{
#ifdef __TESTPAINT__
    testPaintVerticalTunnelHeight = height;
#endif
    session->VerticalTunnelHeight = height / 16;
}

void paint_util_set_general_support_height(paint_session * session, int16_t height, uint8_t slope)
{
    if (session->Support.height >= height) {
        return;
    }

    paint_util_force_set_general_support_height(session, height, slope);
}

void paint_util_force_set_general_support_height(paint_session * session, int16_t height, uint8_t slope)
{
    session->Support.height = height;
    session->Support.slope = slope;
}

const uint16_t segment_offsets[9] = {
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

void paint_util_set_segment_support_height(paint_session * session, int32_t segments, uint16_t height, uint8_t slope)
{
    support_height * supportSegments = session->SupportSegments;
    for (int32_t s = 0; s < 9; s++) {
        if (segments & segment_offsets[s]) {
            supportSegments[s].height = height;
            if (height != 0xFFFF) {
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
