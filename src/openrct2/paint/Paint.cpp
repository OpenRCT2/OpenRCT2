/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include "../config/Config.h"
#include "../core/Math.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../localisation/Localisation.h"
#include "../localisation/LocalisationService.h"
#include "Paint.h"
#include "sprite/Paint.Sprite.h"
#include "tile_element/Paint.TileElement.h"

// Globals for paint clipping
uint8_t gClipHeight = 128; // Default to middle value
LocationXY8 gClipSelectionA = { 0, 0 };
LocationXY8 gClipSelectionB = { MAXIMUM_MAP_SIZE_TECHNICAL - 1, MAXIMUM_MAP_SIZE_TECHNICAL - 1 };

paint_session gPaintSession;
static bool _paintSessionInUse;

static constexpr const uint8_t BoundBoxDebugColours[] =
{
    0,   // NONE
    102, // TERRAIN
    114, // SPRITE
    229, // RIDE
    126, // WATER
    138, // SCENERY
    150, // FOOTPATH
    162, // FOOTPATH_ITEM
    174, // PARK
    186, // WALL
    198, // LARGE_SCENERY
    210, // LABEL
    222, // BANNER
};

bool gShowDirtyVisuals;
bool gPaintBoundingBoxes;
bool gPaintBlockedTiles;

static void paint_session_init(paint_session * session, rct_drawpixelinfo * dpi);
static void paint_attached_ps(rct_drawpixelinfo * dpi, paint_struct * ps, uint32_t viewFlags);
static void paint_ps_image_with_bounding_boxes(rct_drawpixelinfo * dpi, paint_struct * ps, uint32_t imageId, int16_t x, int16_t y);
static void paint_ps_image(rct_drawpixelinfo * dpi, paint_struct * ps, uint32_t imageId, int16_t x, int16_t y);
static uint32_t paint_ps_colourify_image(uint32_t imageId, uint8_t spriteType, uint32_t viewFlags);

static void paint_session_init(paint_session * session, rct_drawpixelinfo * dpi)
{
    session->DPI = dpi;
    session->EndOfPaintStructArray = &session->PaintStructs[4000 - 1];
    session->NextFreePaintStruct = session->PaintStructs;
    session->UnkF1AD28 = nullptr;
    session->UnkF1AD2C = nullptr;
    for (auto &quadrant : session->Quadrants)
    {
        quadrant = nullptr;
    }
    session->QuadrantBackIndex = std::numeric_limits<uint32_t>::max();
    session->QuadrantFrontIndex = 0;
    session->PSStringHead = nullptr;
    session->LastPSString = nullptr;
    session->WoodenSupportsPrependTo = nullptr;
    session->CurrentlyDrawnItem = nullptr;
    session->SurfaceElement = nullptr;
}

static void paint_session_add_ps_to_quadrant(paint_session * session, paint_struct * ps, int32_t positionHash)
{
    uint32_t paintQuadrantIndex = Math::Clamp(0, positionHash / 32, MAX_PAINT_QUADRANTS - 1);
    ps->quadrant_index = paintQuadrantIndex;
    ps->next_quadrant_ps = session->Quadrants[paintQuadrantIndex];
    session->Quadrants[paintQuadrantIndex] = ps;

    session->QuadrantBackIndex = std::min(session->QuadrantBackIndex, paintQuadrantIndex);
    session->QuadrantFrontIndex = std::max(session->QuadrantFrontIndex, paintQuadrantIndex);
}

/**
* Extracted from 0x0098196c, 0x0098197c, 0x0098198c, 0x0098199c
*/
static paint_struct * sub_9819_c(
    paint_session * session, uint32_t image_id, LocationXYZ16 offset, LocationXYZ16 boundBoxSize, LocationXYZ16 boundBoxOffset)
{
    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray) return nullptr;
    auto g1 = gfx_get_g1_element(image_id & 0x7FFFF);
    if (g1 == nullptr)
    {
        return nullptr;
    }

    paint_struct * ps = &session->NextFreePaintStruct->basic;
    ps->image_id = image_id;

    switch (session->CurrentRotation)
    {
    case 0:
        rotate_map_coordinates(&offset.x, &offset.y, 0);
        break;
    case 1:
        rotate_map_coordinates(&offset.x, &offset.y, 3);
        break;
    case 2:
        rotate_map_coordinates(&offset.x, &offset.y, 2);
        break;
    case 3:
        rotate_map_coordinates(&offset.x, &offset.y, 1);
        break;
    }
    offset.x += session->SpritePosition.x;
    offset.y += session->SpritePosition.y;

    LocationXY16 map = coordinate_3d_to_2d(&offset, session->CurrentRotation);

    ps->x = map.x;
    ps->y = map.y;

    int32_t left = map.x + g1->x_offset;
    int32_t bottom = map.y + g1->y_offset;

    int32_t right = left + g1->width;
    int32_t top = bottom + g1->height;

    rct_drawpixelinfo * dpi = session->DPI;

    if (right <= dpi->x)return nullptr;
    if (top <= dpi->y)return nullptr;
    if (left >= dpi->x + dpi->width)return nullptr;
    if (bottom >= dpi->y + dpi->height)return nullptr;


    // This probably rotates the variables so they're relative to rotation 0.
    switch (session->CurrentRotation)
    {
    case 0:
        boundBoxSize.x--;
        boundBoxSize.y--;
        rotate_map_coordinates(&boundBoxOffset.x, &boundBoxOffset.y, 0);
        rotate_map_coordinates(&boundBoxSize.x, &boundBoxSize.y, 0);
        break;
    case 1:
        boundBoxSize.x--;
        rotate_map_coordinates(&boundBoxOffset.x, &boundBoxOffset.y, 3);
        rotate_map_coordinates(&boundBoxSize.x, &boundBoxSize.y, 3);
        break;
    case 2:
        rotate_map_coordinates(&boundBoxSize.x, &boundBoxSize.y, 2);
        rotate_map_coordinates(&boundBoxOffset.x, &boundBoxOffset.y, 2);
        break;
    case 3:
        boundBoxSize.y--;
        rotate_map_coordinates(&boundBoxSize.x, &boundBoxSize.y, 1);
        rotate_map_coordinates(&boundBoxOffset.x, &boundBoxOffset.y, 1);
        break;
    }

    ps->bounds.x_end = boundBoxSize.x + boundBoxOffset.x + session->SpritePosition.x;
    ps->bounds.z = boundBoxOffset.z;
    ps->bounds.z_end = boundBoxOffset.z + boundBoxSize.z;
    ps->bounds.y_end = boundBoxSize.y + boundBoxOffset.y + session->SpritePosition.y;
    ps->flags = 0;
    ps->bounds.x = boundBoxOffset.x + session->SpritePosition.x;
    ps->bounds.y = boundBoxOffset.y + session->SpritePosition.y;
    ps->attached_ps = nullptr;
    ps->var_20 = nullptr;
    ps->sprite_type = session->InteractionType;
    ps->var_29 = 0;
    ps->map_x = session->MapPosition.x;
    ps->map_y = session->MapPosition.y;
    ps->tileElement = (rct_tile_element*)session->CurrentlyDrawnItem;

    return ps;
}

/**
*
*  rct2: 0x0068B6C2
*/
void paint_session_generate(paint_session * session)
{
    rct_drawpixelinfo * dpi = session->DPI;
    LocationXY16 mapTile =
    {
        (int16_t)(dpi->x & 0xFFE0),
        (int16_t)((dpi->y - 16) & 0xFFE0)
    };

    int16_t half_x = mapTile.x >> 1;
    uint16_t num_vertical_quadrants = (dpi->height + 2128) >> 5;

    session->CurrentRotation = get_current_rotation();
    switch (get_current_rotation())
    {
    case 0:
        mapTile.x = mapTile.y - half_x;
        mapTile.y = mapTile.y + half_x;

        mapTile.x &= 0xFFE0;
        mapTile.y &= 0xFFE0;

        for (; num_vertical_quadrants > 0; --num_vertical_quadrants) {
            tile_element_paint_setup(session, mapTile.x, mapTile.y);
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            sprite_paint_setup(session, mapTile.x - 32, mapTile.y + 32);

            tile_element_paint_setup(session, mapTile.x, mapTile.y + 32);
            sprite_paint_setup(session, mapTile.x, mapTile.y + 32);

            mapTile.x += 32;
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            mapTile.y += 32;
        }
        break;
    case 1:
        mapTile.x = -mapTile.y - half_x;
        mapTile.y = mapTile.y - half_x - 16;

        mapTile.x &= 0xFFE0;
        mapTile.y &= 0xFFE0;

        for (; num_vertical_quadrants > 0; --num_vertical_quadrants) {
            tile_element_paint_setup(session, mapTile.x, mapTile.y);
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            sprite_paint_setup(session, mapTile.x - 32, mapTile.y - 32);

            tile_element_paint_setup(session, mapTile.x - 32, mapTile.y);
            sprite_paint_setup(session, mapTile.x - 32, mapTile.y);

            mapTile.y += 32;
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            mapTile.x -= 32;
        }
        break;
    case 2:
        mapTile.x = -mapTile.y + half_x;
        mapTile.y = -mapTile.y - half_x;

        mapTile.x &= 0xFFE0;
        mapTile.y &= 0xFFE0;

        for (; num_vertical_quadrants > 0; --num_vertical_quadrants) {
            tile_element_paint_setup(session, mapTile.x, mapTile.y);
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            sprite_paint_setup(session, mapTile.x + 32, mapTile.y - 32);

            tile_element_paint_setup(session, mapTile.x, mapTile.y - 32);
            sprite_paint_setup(session, mapTile.x, mapTile.y - 32);

            mapTile.x -= 32;

            sprite_paint_setup(session, mapTile.x, mapTile.y);

            mapTile.y -= 32;
        }
        break;
    case 3:
        mapTile.x = mapTile.y + half_x;
        mapTile.y = -mapTile.y + half_x - 16;

        mapTile.x &= 0xFFE0;
        mapTile.y &= 0xFFE0;

        for (; num_vertical_quadrants > 0; --num_vertical_quadrants)
        {
            tile_element_paint_setup(session, mapTile.x, mapTile.y);
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            sprite_paint_setup(session, mapTile.x + 32, mapTile.y + 32);

            tile_element_paint_setup(session, mapTile.x + 32, mapTile.y);
            sprite_paint_setup(session, mapTile.x + 32, mapTile.y);

            mapTile.y -= 32;

            sprite_paint_setup(session, mapTile.x, mapTile.y);

            mapTile.x += 32;
        }
        break;
    }
}

template<uint8_t> static bool check_bounding_box(const paint_struct_bound_box& initialBBox,
    const paint_struct_bound_box& currentBBox)
{
    return false;
}

template<> bool check_bounding_box<0>(const paint_struct_bound_box& initialBBox,
    const paint_struct_bound_box& currentBBox)
{
    if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end >= currentBBox.y && initialBBox.x_end >= currentBBox.x
        && !(initialBBox.z < currentBBox.z_end && initialBBox.y < currentBBox.y_end && initialBBox.x < currentBBox.x_end))
    {
        return true;
    }
    return false;
}

template<> bool check_bounding_box<1>(const paint_struct_bound_box& initialBBox,
    const paint_struct_bound_box& currentBBox)
{
    if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end >= currentBBox.y && initialBBox.x_end < currentBBox.x
        && !(initialBBox.z < currentBBox.z_end && initialBBox.y < currentBBox.y_end && initialBBox.x >= currentBBox.x_end))
    {
        return true;
    }
    return false;
}

template<> bool check_bounding_box<2>(const paint_struct_bound_box& initialBBox,
    const paint_struct_bound_box& currentBBox)
{
    if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end < currentBBox.y && initialBBox.x_end < currentBBox.x
        && !(initialBBox.z < currentBBox.z_end && initialBBox.y >= currentBBox.y_end && initialBBox.x >= currentBBox.x_end))
    {
        return true;
    }
    return false;
}

template<> bool check_bounding_box<3>(const paint_struct_bound_box& initialBBox,
    const paint_struct_bound_box& currentBBox)
{
    if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end < currentBBox.y && initialBBox.x_end >= currentBBox.x
        && !(initialBBox.z < currentBBox.z_end && initialBBox.y >= currentBBox.y_end && initialBBox.x < currentBBox.x_end))
    {
        return true;
    }
    return false;
}

template<uint8_t _TRotation>
static paint_struct * paint_arrange_structs_helper_rotation(paint_struct * ps_next, uint16_t quadrantIndex, uint8_t flag)
{
    paint_struct * ps;
    paint_struct * ps_temp;
    do
    {
        ps = ps_next;
        ps_next = ps_next->next_quadrant_ps;
        if (ps_next == nullptr) return ps;
    } while (quadrantIndex > ps_next->quadrant_index);

    // Cache the last visited node so we don't have to walk the whole list again
    paint_struct * ps_cache = ps;

    ps_temp = ps;
    do {
        ps = ps->next_quadrant_ps;
        if (ps == nullptr) break;

        if (ps->quadrant_index > quadrantIndex + 1)
        {
            ps->quadrant_flags = PAINT_QUADRANT_FLAG_BIGGER;
        }
        else if (ps->quadrant_index == quadrantIndex + 1)
        {
            ps->quadrant_flags = PAINT_QUADRANT_FLAG_NEXT | PAINT_QUADRANT_FLAG_IDENTICAL;
        }
        else if (ps->quadrant_index == quadrantIndex)
        {
            ps->quadrant_flags = flag | PAINT_QUADRANT_FLAG_IDENTICAL;
        }
    } while (ps->quadrant_index <= quadrantIndex + 1);
    ps = ps_temp;

    while (true)
    {
        while (true)
        {
            ps_next = ps->next_quadrant_ps;
            if (ps_next == nullptr) return ps_cache;
            if (ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_BIGGER) return ps_cache;
            if (ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_IDENTICAL) break;
            ps = ps_next;
        }

        ps_next->quadrant_flags &= ~PAINT_QUADRANT_FLAG_IDENTICAL;
        ps_temp = ps;

        const paint_struct_bound_box& initialBBox = ps_next->bounds;

        while (true)
        {
            ps = ps_next;
            ps_next = ps_next->next_quadrant_ps;
            if (ps_next == nullptr) break;
            if (ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_BIGGER) break;
            if (!(ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_NEXT)) continue;

            const paint_struct_bound_box& currentBBox = ps_next->bounds;

            const bool compareResult = check_bounding_box<_TRotation>(initialBBox, currentBBox);

            if (compareResult)
            {
                ps->next_quadrant_ps = ps_next->next_quadrant_ps;
                paint_struct *ps_temp2 = ps_temp->next_quadrant_ps;
                ps_temp->next_quadrant_ps = ps_next;
                ps_next->next_quadrant_ps = ps_temp2;
                ps_next = ps;
            }
        }

        ps = ps_temp;
    }
}

paint_struct * paint_arrange_structs_helper(paint_struct * ps_next, uint16_t quadrantIndex, uint8_t flag, uint8_t rotation)
{
    switch (rotation)
    {
    case 0:
        return paint_arrange_structs_helper_rotation<0>(ps_next, quadrantIndex, flag);
    case 1:
        return paint_arrange_structs_helper_rotation<1>(ps_next, quadrantIndex, flag);
    case 2:
        return paint_arrange_structs_helper_rotation<2>(ps_next, quadrantIndex, flag);
    case 3:
        return paint_arrange_structs_helper_rotation<3>(ps_next, quadrantIndex, flag);
    }
    return nullptr;
}

/**
*
*  rct2: 0x00688217
*/
paint_struct paint_session_arrange(paint_session * session)
{
    paint_struct psHead = {};
    paint_struct * ps = &psHead;
    ps->next_quadrant_ps = nullptr;
    uint32_t quadrantIndex = session->QuadrantBackIndex;
    const uint8_t rotation = get_current_rotation();
    if (quadrantIndex != UINT32_MAX)
    {
        do
        {
            paint_struct * ps_next = session->Quadrants[quadrantIndex];
            if (ps_next != nullptr)
            {
                ps->next_quadrant_ps = ps_next;
                do
                {
                    ps = ps_next;
                    ps_next = ps_next->next_quadrant_ps;

                } while (ps_next != nullptr);
            }
        } while (++quadrantIndex <= session->QuadrantFrontIndex);

        paint_struct * ps_cache = paint_arrange_structs_helper(&psHead, session->QuadrantBackIndex & 0xFFFF, PAINT_QUADRANT_FLAG_NEXT, rotation);

        quadrantIndex = session->QuadrantBackIndex;
        while (++quadrantIndex < session->QuadrantFrontIndex)
        {
            ps_cache = paint_arrange_structs_helper(ps_cache, quadrantIndex & 0xFFFF, 0, rotation);
        }
    }

    return psHead;
}

/**
*
*  rct2: 0x00688485
*/
void paint_draw_structs(rct_drawpixelinfo * dpi, paint_struct * ps, uint32_t viewFlags)
{
    paint_struct* previous_ps = ps->next_quadrant_ps;
    for (ps = ps->next_quadrant_ps; ps;)
    {
        int16_t x = ps->x;
        int16_t y = ps->y;
        if (ps->sprite_type == VIEWPORT_INTERACTION_ITEM_SPRITE)
        {
            if (dpi->zoom_level >= 1)
            {
                x = floor2(x, 2);
                y = floor2(y, 2);
                if (dpi->zoom_level >= 2)
                {
                    x = floor2(x, 4);
                    y = floor2(y, 4);
                }
            }
        }

        uint32_t imageId = paint_ps_colourify_image(ps->image_id, ps->sprite_type, viewFlags);
        if (gPaintBoundingBoxes && dpi->zoom_level == 0)
        {
            paint_ps_image_with_bounding_boxes(dpi, ps, imageId, x, y);
        }
        else
        {
            paint_ps_image(dpi, ps, imageId, x, y);
        }

        if (ps->var_20 != nullptr)
        {
            // NOTE: RCT uses var_20 instead of next_quadrant_ps, do we still need it?
            ps = ps->var_20;
        }
        else
        {
            paint_attached_ps(dpi, ps, viewFlags);
            ps = previous_ps->next_quadrant_ps;
            previous_ps = ps;
        }
    }
}

/**
*
*  rct2: 0x00688596
*  Part of 0x688485
*/
static void paint_attached_ps(rct_drawpixelinfo * dpi, paint_struct * ps, uint32_t viewFlags)
{
    attached_paint_struct * attached_ps = ps->attached_ps;
    for (; attached_ps; attached_ps = attached_ps->next)
    {
        int16_t x = attached_ps->x + ps->x;
        int16_t y = attached_ps->y + ps->y;

        uint32_t imageId = paint_ps_colourify_image(attached_ps->image_id, ps->sprite_type, viewFlags);
        if (attached_ps->flags & PAINT_STRUCT_FLAG_IS_MASKED)
        {
            gfx_draw_sprite_raw_masked(dpi, x, y, imageId, attached_ps->colour_image_id);
        }
        else
        {
            gfx_draw_sprite(dpi, imageId, x, y, ps->tertiary_colour);
        }
    }
}

static void paint_ps_image_with_bounding_boxes(rct_drawpixelinfo * dpi, paint_struct * ps, uint32_t imageId, int16_t x, int16_t y)
{
    const uint8_t colour = BoundBoxDebugColours[ps->sprite_type];
    const uint8_t rotation = get_current_rotation();

    const LocationXYZ16 frontTop =
    {
        (int16_t)ps->bounds.x_end,
        (int16_t)ps->bounds.y_end,
        (int16_t)ps->bounds.z_end
    };
    const LocationXY16 screenCoordFrontTop = coordinate_3d_to_2d(&frontTop, rotation);

    const LocationXYZ16 frontBottom =
    {
        (int16_t)ps->bounds.x_end,
        (int16_t)ps->bounds.y_end,
        (int16_t)ps->bounds.z
    };
    const LocationXY16 screenCoordFrontBottom = coordinate_3d_to_2d(&frontBottom, rotation);

    const LocationXYZ16 leftTop =
    {
        (int16_t)ps->bounds.x,
        (int16_t)ps->bounds.y_end,
        (int16_t)ps->bounds.z_end
    };
    const LocationXY16 screenCoordLeftTop = coordinate_3d_to_2d(&leftTop, rotation);

    const LocationXYZ16 leftBottom =
    {
        (int16_t)ps->bounds.x,
        (int16_t)ps->bounds.y_end,
        (int16_t)ps->bounds.z
    };
    const LocationXY16 screenCoordLeftBottom = coordinate_3d_to_2d(&leftBottom, rotation);

    const LocationXYZ16 rightTop =
    {
        (int16_t)ps->bounds.x_end,
        (int16_t)ps->bounds.y,
        (int16_t)ps->bounds.z_end
    };
    const LocationXY16 screenCoordRightTop = coordinate_3d_to_2d(&rightTop, rotation);

    const LocationXYZ16 rightBottom =
    {
        (int16_t)ps->bounds.x_end,
        (int16_t)ps->bounds.y,
        (int16_t)ps->bounds.z
    };
    const LocationXY16 screenCoordRightBottom = coordinate_3d_to_2d(&rightBottom, rotation);

    const LocationXYZ16 backTop =
    {
        (int16_t)ps->bounds.x,
        (int16_t)ps->bounds.y,
        (int16_t)ps->bounds.z_end
    };
    const LocationXY16 screenCoordBackTop = coordinate_3d_to_2d(&backTop, rotation);

    const LocationXYZ16 backBottom =
    {
        (int16_t)ps->bounds.x,
        (int16_t)ps->bounds.y,
        (int16_t)ps->bounds.z
    };
    const LocationXY16 screenCoordBackBottom = coordinate_3d_to_2d(&backBottom, rotation);

    // bottom square
    gfx_draw_line(dpi, screenCoordFrontBottom.x, screenCoordFrontBottom.y, screenCoordLeftBottom.x, screenCoordLeftBottom.y, colour);
    gfx_draw_line(dpi, screenCoordBackBottom.x, screenCoordBackBottom.y, screenCoordLeftBottom.x, screenCoordLeftBottom.y, colour);
    gfx_draw_line(dpi, screenCoordBackBottom.x, screenCoordBackBottom.y, screenCoordRightBottom.x, screenCoordRightBottom.y, colour);
    gfx_draw_line(dpi, screenCoordFrontBottom.x, screenCoordFrontBottom.y, screenCoordRightBottom.x, screenCoordRightBottom.y, colour);

    //vertical back + sides
    gfx_draw_line(dpi, screenCoordBackTop.x, screenCoordBackTop.y, screenCoordBackBottom.x, screenCoordBackBottom.y, colour);
    gfx_draw_line(dpi, screenCoordLeftTop.x, screenCoordLeftTop.y, screenCoordLeftBottom.x, screenCoordLeftBottom.y, colour);
    gfx_draw_line(dpi, screenCoordRightTop.x, screenCoordRightTop.y, screenCoordRightBottom.x, screenCoordRightBottom.y, colour);

    // top square back
    gfx_draw_line(dpi, screenCoordBackTop.x, screenCoordBackTop.y, screenCoordLeftTop.x, screenCoordLeftTop.y, colour);
    gfx_draw_line(dpi, screenCoordBackTop.x, screenCoordBackTop.y, screenCoordRightTop.x, screenCoordRightTop.y, colour);

    paint_ps_image(dpi, ps, imageId, x, y);

    // vertical front
    gfx_draw_line(dpi, screenCoordFrontTop.x, screenCoordFrontTop.y, screenCoordFrontBottom.x, screenCoordFrontBottom.y, colour);

    // top square
    gfx_draw_line(dpi, screenCoordFrontTop.x, screenCoordFrontTop.y, screenCoordLeftTop.x, screenCoordLeftTop.y, colour);
    gfx_draw_line(dpi, screenCoordFrontTop.x, screenCoordFrontTop.y, screenCoordRightTop.x, screenCoordRightTop.y, colour);
}

static void paint_ps_image(rct_drawpixelinfo * dpi, paint_struct * ps, uint32_t imageId, int16_t x, int16_t y)
{
    if (ps->flags & PAINT_STRUCT_FLAG_IS_MASKED)
    {
        return gfx_draw_sprite_raw_masked(dpi, x, y, imageId, ps->colour_image_id);
    }

    gfx_draw_sprite(dpi, imageId, x, y, ps->tertiary_colour);
}

static uint32_t paint_ps_colourify_image(uint32_t imageId, uint8_t spriteType, uint32_t viewFlags)
{
    constexpr uint32_t primaryColour   = COLOUR_BRIGHT_YELLOW;
    constexpr uint32_t secondaryColour = COLOUR_GREY;
    constexpr uint32_t seeThoughFlags  = IMAGE_TYPE_TRANSPARENT | (primaryColour << 19) | (secondaryColour << 24);

    if (viewFlags & VIEWPORT_FLAG_SEETHROUGH_RIDES)
    {
        if (spriteType == VIEWPORT_INTERACTION_ITEM_RIDE)
        {
            imageId &= 0x7FFFF;
            imageId |= seeThoughFlags;
        }
    }
    if (viewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
    {
        if (spriteType == VIEWPORT_INTERACTION_ITEM_WALL)
        {
            imageId &= 0x7FFFF;
            imageId |= seeThoughFlags;
        }
    }
    if (viewFlags & VIEWPORT_FLAG_SEETHROUGH_PATHS)
    {
        switch (spriteType)
        {
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
        case VIEWPORT_INTERACTION_ITEM_BANNER:
            imageId &= 0x7FFFF;
            imageId |= seeThoughFlags;
            break;
        }
    }
    if (viewFlags & VIEWPORT_FLAG_SEETHROUGH_SCENERY)
    {
        switch (spriteType)
        {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        case VIEWPORT_INTERACTION_ITEM_WALL:
            imageId &= 0x7FFFF;
            imageId |= seeThoughFlags;
            break;
        }
    }
    return imageId;
}

static void draw_pixel_info_crop_by_zoom(rct_drawpixelinfo *dpi)
{
    int32_t zoom = dpi->zoom_level;
    dpi->zoom_level = 0;
    dpi->x >>= zoom;
    dpi->y >>= zoom;
    dpi->width >>= zoom;
    dpi->height >>= zoom;
}

paint_session * paint_session_alloc(rct_drawpixelinfo * dpi)
{
    // Currently limited to just one session at a time
    assert(!_paintSessionInUse);
    _paintSessionInUse = true;
    paint_session * session = &gPaintSession;

    paint_session_init(session, dpi);
    return session;
}

void paint_session_free([[maybe_unused]] paint_session* session)
{
    _paintSessionInUse = false;
}

/**
*  rct2: 0x006861AC, 0x00686337, 0x006864D0, 0x0068666B, 0x0098196C
*
* @param image_id (ebx)
* @param x_offset (al)
* @param y_offset (cl)
* @param bound_box_length_x (di)
* @param bound_box_length_y (si)
* @param bound_box_length_z (ah)
* @param z_offset (dx)
* @return (ebp) paint_struct on success (CF == 0), nullptr on failure (CF == 1)
*/
paint_struct * sub_98196C(
    paint_session * session,
    uint32_t          image_id,
    int8_t           x_offset,
    int8_t           y_offset,
    int16_t          bound_box_length_x,
    int16_t          bound_box_length_y,
    int8_t           bound_box_length_z,
    int16_t          z_offset)
{
    assert((uint16_t)bound_box_length_x == (int16_t)bound_box_length_x);
    assert((uint16_t)bound_box_length_y == (int16_t)bound_box_length_y);

    session->UnkF1AD28 = nullptr;
    session->UnkF1AD2C = nullptr;

    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray)
    {
        return nullptr;
    }

    auto g1Element = gfx_get_g1_element(image_id & 0x7FFFF);
    if (g1Element == nullptr)
    {
        return nullptr;
    }

    paint_struct *ps = &session->NextFreePaintStruct->basic;
    ps->image_id = image_id;

    LocationXYZ16 coord_3d =
    {
        x_offset, // ax
        y_offset, // cx
        z_offset
    };

    LocationXYZ16 boundBox =
    {
        bound_box_length_x, // di
        bound_box_length_y, // si
        bound_box_length_z,
    };

    switch (session->CurrentRotation)
    {
    case 0:
        rotate_map_coordinates(&coord_3d.x, &coord_3d.y, TILE_ELEMENT_DIRECTION_WEST);

        boundBox.x--;
        boundBox.y--;
        rotate_map_coordinates(&boundBox.x, &boundBox.y, TILE_ELEMENT_DIRECTION_WEST);
        break;

    case 1:
        rotate_map_coordinates(&coord_3d.x, &coord_3d.y, TILE_ELEMENT_DIRECTION_SOUTH);

        boundBox.x--;
        rotate_map_coordinates(&boundBox.x, &boundBox.y, TILE_ELEMENT_DIRECTION_SOUTH);
        break;

    case 2:
        rotate_map_coordinates(&coord_3d.x, &coord_3d.y, TILE_ELEMENT_DIRECTION_EAST);
        rotate_map_coordinates(&boundBox.x, &boundBox.y, TILE_ELEMENT_DIRECTION_EAST);
        break;

    case 3:
        rotate_map_coordinates(&coord_3d.x, &coord_3d.y, TILE_ELEMENT_DIRECTION_NORTH);

        boundBox.y--;
        rotate_map_coordinates(&boundBox.x, &boundBox.y, TILE_ELEMENT_DIRECTION_NORTH);
        break;
    }

    coord_3d.x += session->SpritePosition.x;
    coord_3d.y += session->SpritePosition.y;

    ps->bounds.x_end = coord_3d.x + boundBox.x;
    ps->bounds.y_end = coord_3d.y + boundBox.y;

    // TODO: check whether this is right. edx is ((bound_box_length_z + z_offset) << 16 | z_offset)
    ps->bounds.z = coord_3d.z;
    ps->bounds.z_end = (boundBox.z + coord_3d.z);

    LocationXY16 map = coordinate_3d_to_2d(&coord_3d, session->CurrentRotation);

    ps->x = map.x;
    ps->y = map.y;

    int16_t left = map.x + g1Element->x_offset;
    int16_t bottom = map.y + g1Element->y_offset;

    int16_t right = left + g1Element->width;
    int16_t top = bottom + g1Element->height;

    rct_drawpixelinfo *dpi = session->DPI;

    if (right <= dpi->x) return nullptr;
    if (top <= dpi->y) return nullptr;
    if (left >= (dpi->x + dpi->width)) return nullptr;
    if (bottom >= (dpi->y + dpi->height)) return nullptr;

    ps->flags = 0;
    ps->bounds.x = coord_3d.x;
    ps->bounds.y = coord_3d.y;
    ps->attached_ps = nullptr;
    ps->var_20 = nullptr;
    ps->sprite_type = session->InteractionType;
    ps->var_29 = 0;
    ps->map_x = session->MapPosition.x;
    ps->map_y = session->MapPosition.y;
    ps->tileElement = (rct_tile_element*)session->CurrentlyDrawnItem;

    session->UnkF1AD28 = ps;

    int32_t positionHash = 0;
    switch (session->CurrentRotation)
    {
    case 0:
        positionHash = coord_3d.y + coord_3d.x;
        break;
    case 1:
        positionHash = coord_3d.y - coord_3d.x + 0x2000;
        break;
    case 2:
        positionHash = -(coord_3d.y + coord_3d.x) + 0x4000;
        break;
    case 3:
        positionHash = coord_3d.x - coord_3d.y + 0x2000;
        break;
    }
    paint_session_add_ps_to_quadrant(session, ps, positionHash);

    session->NextFreePaintStruct++;

    return ps;
}

/**
*  rct2: 0x00686806, 0x006869B2, 0x00686B6F, 0x00686D31, 0x0098197C
*
* @param image_id (ebx)
* @param x_offset (al)
* @param y_offset (cl)
* @param bound_box_length_x (di)
* @param bound_box_length_y (si)
* @param bound_box_length_z (ah)
* @param z_offset (dx)
* @param bound_box_offset_x (0x009DEA52)
* @param bound_box_offset_y (0x009DEA54)
* @param bound_box_offset_z (0x009DEA56)
* @return (ebp) paint_struct on success (CF == 0), nullptr on failure (CF == 1)
*/
// Track Pieces, Shops.
paint_struct * sub_98197C(
    paint_session * session,
    uint32_t          image_id,
    int8_t           x_offset,
    int8_t           y_offset,
    int16_t          bound_box_length_x,
    int16_t          bound_box_length_y,
    int8_t           bound_box_length_z,
    int16_t          z_offset,
    int16_t          bound_box_offset_x,
    int16_t          bound_box_offset_y,
    int16_t          bound_box_offset_z)
{
    session->UnkF1AD28 = nullptr;
    session->UnkF1AD2C = nullptr;

    LocationXYZ16 offset = { x_offset, y_offset, z_offset };
    LocationXYZ16 boundBoxSize = { bound_box_length_x, bound_box_length_y, bound_box_length_z };
    LocationXYZ16 boundBoxOffset = { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z };
    paint_struct * ps = sub_9819_c(session, image_id, offset, boundBoxSize, boundBoxOffset);

    if (ps == nullptr) {
        return nullptr;
    }

    session->UnkF1AD28 = ps;

    LocationXY16 attach =
    {
        (int16_t)ps->bounds.x,
        (int16_t)ps->bounds.y
    };

    rotate_map_coordinates(&attach.x, &attach.y, session->CurrentRotation);
    switch (session->CurrentRotation)
    {
    case 0:
        break;
    case 1:
    case 3:
        attach.x += 0x2000;
        break;
    case 2:
        attach.x += 0x4000;
        break;
    }

    int32_t positionHash = attach.x + attach.y;
    paint_session_add_ps_to_quadrant(session, ps, positionHash);

    session->NextFreePaintStruct++;
    return ps;
}

/**
*
*  rct2: 0x00686EF0, 0x00687056, 0x006871C8, 0x0068733C, 0x0098198C
*
* @param image_id (ebx)
* @param x_offset (al)
* @param y_offset (cl)
* @param bound_box_length_x (di)
* @param bound_box_length_y (si)
* @param bound_box_length_z (ah)
* @param z_offset (dx)
* @param bound_box_offset_x (0x009DEA52)
* @param bound_box_offset_y (0x009DEA54)
* @param bound_box_offset_z (0x009DEA56)
* @return (ebp) paint_struct on success (CF == 0), nullptr on failure (CF == 1)
*/
paint_struct * sub_98198C(
    paint_session * session,
    uint32_t          image_id,
    int8_t           x_offset,
    int8_t           y_offset,
    int16_t          bound_box_length_x,
    int16_t          bound_box_length_y,
    int8_t           bound_box_length_z,
    int16_t          z_offset,
    int16_t          bound_box_offset_x,
    int16_t          bound_box_offset_y,
    int16_t          bound_box_offset_z)
{
    assert((uint16_t)bound_box_length_x == bound_box_length_x);
    assert((uint16_t)bound_box_length_y == bound_box_length_y);

    session->UnkF1AD28 = nullptr;
    session->UnkF1AD2C = nullptr;

    LocationXYZ16 offset = { x_offset, y_offset, z_offset };
    LocationXYZ16 boundBoxSize = { bound_box_length_x, bound_box_length_y, bound_box_length_z };
    LocationXYZ16 boundBoxOffset = { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z };
    paint_struct * ps = sub_9819_c(session, image_id, offset, boundBoxSize, boundBoxOffset);

    if (ps == nullptr) {
        return nullptr;
    }

    session->UnkF1AD28 = ps;
    session->NextFreePaintStruct++;
    return ps;
}

/**
*
*  rct2: 0x006874B0, 0x00687618, 0x0068778C, 0x00687902, 0x0098199C
*
* @param image_id (ebx)
* @param x_offset (al)
* @param y_offset (cl)
* @param bound_box_length_x (di)
* @param bound_box_length_y (si)
* @param bound_box_length_z (ah)
* @param z_offset (dx)
* @param bound_box_offset_x (0x009DEA52)
* @param bound_box_offset_y (0x009DEA54)
* @param bound_box_offset_z (0x009DEA56)
* @return (ebp) paint_struct on success (CF == 0), nullptr on failure (CF == 1)
*/
paint_struct * sub_98199C(
    paint_session * session,
    uint32_t          image_id,
    int8_t           x_offset,
    int8_t           y_offset,
    int16_t          bound_box_length_x,
    int16_t          bound_box_length_y,
    int8_t           bound_box_length_z,
    int16_t          z_offset,
    int16_t          bound_box_offset_x,
    int16_t          bound_box_offset_y,
    int16_t          bound_box_offset_z)
{
    assert((uint16_t)bound_box_length_x == (int16_t)bound_box_length_x);
    assert((uint16_t)bound_box_length_y == (int16_t)bound_box_length_y);

    if (session->UnkF1AD28 == nullptr)
    {
        return sub_98197C(
            session, image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset,
            bound_box_offset_x, bound_box_offset_y, bound_box_offset_z);
    }

    LocationXYZ16 offset = { x_offset, y_offset, z_offset };
    LocationXYZ16 boundBox = { bound_box_length_x, bound_box_length_y, bound_box_length_z };
    LocationXYZ16 boundBoxOffset = { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z };
    paint_struct * ps = sub_9819_c(session, image_id, offset, boundBox, boundBoxOffset);

    if (ps == nullptr)
    {
        return nullptr;
    }

    paint_struct *old_ps = session->UnkF1AD28;
    old_ps->var_20 = ps;

    session->UnkF1AD28 = ps;
    session->NextFreePaintStruct++;
    return ps;
}

/**
* rct2: 0x006881D0
*
* @param image_id (ebx)
* @param x (ax)
* @param y (cx)
* @return (!CF) success
*/
bool paint_attach_to_previous_attach(paint_session * session, uint32_t image_id, uint16_t x, uint16_t y)
{
    if (session->UnkF1AD2C == nullptr)
    {
        return paint_attach_to_previous_ps(session, image_id, x, y);
    }

    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray)
    {
        return false;
    }
    attached_paint_struct * ps = &session->NextFreePaintStruct->attached;
    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;

    attached_paint_struct * ebx = session->UnkF1AD2C;

    ps->next = nullptr;
    ebx->next = ps;

    session->UnkF1AD2C = ps;

    session->NextFreePaintStruct++;

    return true;
}

/**
* rct2: 0x0068818E
*
* @param image_id (ebx)
* @param x (ax)
* @param y (cx)
* @return (!CF) success
*/
bool paint_attach_to_previous_ps(paint_session * session, uint32_t image_id, uint16_t x, uint16_t y)
{
    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray)
    {
        return false;
    }
    attached_paint_struct * ps = &session->NextFreePaintStruct->attached;

    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;

    paint_struct * masterPs = session->UnkF1AD28;
    if (masterPs == nullptr)
    {
        return false;
    }

    session->NextFreePaintStruct++;

    attached_paint_struct * oldFirstAttached = masterPs->attached_ps;
    masterPs->attached_ps = ps;

    ps->next = oldFirstAttached;

    session->UnkF1AD2C = ps;

    return true;
}

/**
* rct2: 0x00685EBC, 0x00686046, 0x00685FC8, 0x00685F4A, 0x00685ECC
* @param amount (eax)
* @param string_id (bx)
* @param y (cx)
* @param z (dx)
* @param offset_x (si)
* @param y_offsets (di)
* @param rotation (ebp)
*/
void paint_floating_money_effect(paint_session * session, money32 amount, rct_string_id string_id, int16_t y, int16_t z, int8_t y_offsets[], int16_t offset_x, uint32_t rotation)
{
    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray)
    {
        return;
    }

    paint_string_struct * ps = &session->NextFreePaintStruct->string;
    ps->string_id = string_id;
    ps->next = nullptr;
    ps->args[0] = amount;
    ps->args[1] = y;
    ps->args[2] = 0;
    ps->args[3] = 0;
    ps->y_offsets = (uint8_t *)y_offsets;

    const LocationXYZ16 position =
    {
        session->SpritePosition.x,
        session->SpritePosition.y,
        z
    };
    const LocationXY16 coord = coordinate_3d_to_2d(&position, rotation);

    ps->x = coord.x + offset_x;
    ps->y = coord.y;

    session->NextFreePaintStruct++;

    if (session->LastPSString == nullptr)
    {
        session->PSStringHead = ps;
    }
    else
    {
        session->LastPSString->next = ps;
    }
    session->LastPSString = ps;
}

/**
*
*  rct2: 0x006860C3
*/
void paint_draw_money_structs(rct_drawpixelinfo * dpi, paint_string_struct * ps)
{
    rct_drawpixelinfo dpi2 = *dpi;
    draw_pixel_info_crop_by_zoom(&dpi2);

    do
    {
        utf8 buffer[256];
        format_string(buffer, 256, ps->string_id, &ps->args);
        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

        // Use sprite font unless the currency contains characters unsupported by the sprite font
        bool forceSpriteFont = false;
        const currency_descriptor& currencyDesc = CurrencyDescriptors[gConfigGeneral.currency_format];
        if (LocalisationService_UseTrueTypeFont() && font_supports_string_sprite(currencyDesc.symbol_unicode))
        {
            forceSpriteFont = true;
        }

        gfx_draw_string_with_y_offsets(&dpi2, buffer, COLOUR_BLACK, ps->x, ps->y, (int8_t *)ps->y_offsets, forceSpriteFont);
    } while ((ps = ps->next) != nullptr);
}
