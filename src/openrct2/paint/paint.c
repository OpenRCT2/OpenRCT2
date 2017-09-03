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

#include "paint.h"
#include "../drawing/drawing.h"
#include "../localisation/localisation.h"
#include "../config/Config.h"
#include "../interface/viewport.h"
#include "map_element/map_element.h"
#include "sprite/sprite.h"
#include "supports.h"

// Global for paint clipping height
uint8 gClipHeight = 128; // Default to middle value

const uint32 construction_markers[] = {
    COLOUR_DARK_GREEN << 19 | COLOUR_GREY << 24 | IMAGE_TYPE_REMAP, // White
    PALETTE_DARKEN_2 << 19 | IMAGE_TYPE_TRANSPARENT, // Translucent
};

paint_session gPaintSession;
static bool _paintSessionInUse;

#ifndef NO_RCT2
#define _paintQuadrants (RCT2_ADDRESS(0x00F1A50C, paint_struct*))
#define _paintQuadrantBackIndex RCT2_GLOBAL(0xF1AD0C, uint32)
#define _paintQuadrantFrontIndex RCT2_GLOBAL(0xF1AD10, uint32)
#endif

static const uint8 BoundBoxDebugColours[] = {
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

static void paint_session_init(paint_session * session, rct_drawpixelinfo * dpi);
static void paint_attached_ps(rct_drawpixelinfo * dpi, paint_struct * ps, uint32 viewFlags);
static void paint_ps_image_with_bounding_boxes(rct_drawpixelinfo * dpi, paint_struct * ps, uint32 imageId, sint16 x, sint16 y);
static void paint_ps_image(rct_drawpixelinfo * dpi, paint_struct * ps, uint32 imageId, sint16 x, sint16 y);
static uint32 paint_ps_colourify_image(uint32 imageId, uint8 spriteType, uint32 viewFlags);

paint_session * paint_session_alloc(rct_drawpixelinfo * dpi)
{
    // Currently limited to just one session at a time
    assert(!_paintSessionInUse);
    _paintSessionInUse = true;
    paint_session * session = &gPaintSession;

    paint_session_init(session, dpi);
    return session;
}

void paint_session_free(paint_session * session)
{
    _paintSessionInUse = false;
}

static void paint_session_init(paint_session * session, rct_drawpixelinfo * dpi)
{
    memset(session, 0, sizeof(paint_session));
    session->Unk140E9A8 = dpi;
    session->EndOfPaintStructArray = &session->PaintStructs[4000 - 1];
    session->NextFreePaintStruct = session->PaintStructs;
    session->UnkF1AD28 = NULL;
    session->UnkF1AD2C = NULL;
    for (sint32 i = 0; i < MAX_PAINT_QUADRANTS; i++) {
        session->Quadrants[i] = NULL;
    }
    session->QuadrantBackIndex = -1;
    session->QuadrantFrontIndex = 0;
    session->PSStringHead = NULL;
    session->LastPSString = NULL;
    session->WoodenSupportsPrependTo = NULL;
}

static void paint_session_add_ps_to_quadrant(paint_session * session, paint_struct * ps, sint32 positionHash)
{
    uint32 paintQuadrantIndex = clamp(0, positionHash / 32, MAX_PAINT_QUADRANTS - 1);
    ps->quadrant_index = paintQuadrantIndex;
    ps->next_quadrant_ps = session->Quadrants[paintQuadrantIndex];
    session->Quadrants[paintQuadrantIndex] = ps;

    session->QuadrantBackIndex = min(session->QuadrantBackIndex, paintQuadrantIndex);
    session->QuadrantFrontIndex = max(session->QuadrantFrontIndex, paintQuadrantIndex);
}

/**
 * Extracted from 0x0098196c, 0x0098197c, 0x0098198c, 0x0098199c
 */
static paint_struct * sub_9819_c(paint_session * session, uint32 image_id, rct_xyz16 offset, rct_xyz16 boundBoxSize, rct_xyz16 boundBoxOffset, uint8 rotation)
{
    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray) return NULL;
    paint_struct * ps = &session->NextFreePaintStruct->basic;

    ps->image_id = image_id;

    uint32 image_element = image_id & 0x7FFFF;
    rct_g1_element * g1Element = gfx_get_g1_element(image_element);

    switch (rotation) {
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

    rct_xy16 map = coordinate_3d_to_2d(&offset, rotation);

    ps->x = map.x;
    ps->y = map.y;

    sint32 left = map.x + g1Element->x_offset;
    sint32 bottom = map.y + g1Element->y_offset;

    sint32 right = left + g1Element->width;
    sint32 top = bottom + g1Element->height;

    rct_drawpixelinfo * dpi = session->Unk140E9A8;

    if (right <= dpi->x)return NULL;
    if (top <= dpi->y)return NULL;
    if (left >= dpi->x + dpi->width)return NULL;
    if (bottom >= dpi->y + dpi->height)return NULL;


    // This probably rotates the variables so they're relative to rotation 0.
    switch (rotation) {
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

    ps->bound_box_x_end = boundBoxSize.x + boundBoxOffset.x + session->SpritePosition.x;
    ps->bound_box_z = boundBoxOffset.z;
    ps->bound_box_z_end = boundBoxOffset.z + boundBoxSize.z;
    ps->bound_box_y_end = boundBoxSize.y + boundBoxOffset.y + session->SpritePosition.y;
    ps->flags = 0;
    ps->bound_box_x = boundBoxOffset.x + session->SpritePosition.x;
    ps->bound_box_y = boundBoxOffset.y + session->SpritePosition.y;
    ps->attached_ps = NULL;
    ps->var_20 = NULL;
    ps->sprite_type = session->InteractionType;
    ps->var_29 = 0;
    ps->map_x = session->MapPosition.x;
    ps->map_y = session->MapPosition.y;
    ps->mapElement = session->CurrentlyDrawnItem;

    return ps;
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
 * @param rotation (ebp)
 * @return (ebp) paint_struct on success (CF == 0), NULL on failure (CF == 1)
 */
paint_struct * sub_98196C(
    paint_session * session,
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    uint32 rotation
) {
    assert((uint16) bound_box_length_x == (sint16) bound_box_length_x);
    assert((uint16) bound_box_length_y == (sint16) bound_box_length_y);

    session->UnkF1AD28 = 0;
    session->UnkF1AD2C = NULL;

    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray) {
        return NULL;
    }

    paint_struct *ps = &session->NextFreePaintStruct->basic;
    ps->image_id = image_id;

    uint32 image_element = image_id & 0x7FFFF;
    rct_g1_element *g1Element = gfx_get_g1_element(image_element);

    rct_xyz16 coord_3d = {
        .x = x_offset, // ax
        .y = y_offset, // cx
        .z = z_offset
    };

    rct_xyz16 boundBox = {
        .x = bound_box_length_x, // di
        .y = bound_box_length_y, // si
        .z = bound_box_length_z,
    };

    switch (rotation) {
        case 0:
            rotate_map_coordinates(&coord_3d.x, &coord_3d.y, MAP_ELEMENT_DIRECTION_WEST);

            boundBox.x--;
            boundBox.y--;
            rotate_map_coordinates(&boundBox.x, &boundBox.y, MAP_ELEMENT_DIRECTION_WEST);
            break;

        case 1:
            rotate_map_coordinates(&coord_3d.x, &coord_3d.y, MAP_ELEMENT_DIRECTION_SOUTH);

            boundBox.x--;
            rotate_map_coordinates(&boundBox.x, &boundBox.y, MAP_ELEMENT_DIRECTION_SOUTH);
            break;

        case 2:
            rotate_map_coordinates(&coord_3d.x, &coord_3d.y, MAP_ELEMENT_DIRECTION_EAST);
            rotate_map_coordinates(&boundBox.x, &boundBox.y, MAP_ELEMENT_DIRECTION_EAST);
            break;

        case 3:
            rotate_map_coordinates(&coord_3d.x, &coord_3d.y, MAP_ELEMENT_DIRECTION_NORTH);

            boundBox.y--;
            rotate_map_coordinates(&boundBox.x, &boundBox.y, MAP_ELEMENT_DIRECTION_NORTH);
            break;
    }

    coord_3d.x += session->SpritePosition.x;
    coord_3d.y += session->SpritePosition.y;

    ps->bound_box_x_end = coord_3d.x + boundBox.x;
    ps->bound_box_y_end = coord_3d.y + boundBox.y;

    // TODO: check whether this is right. edx is ((bound_box_length_z + z_offset) << 16 | z_offset)
    ps->bound_box_z = coord_3d.z;
    ps->bound_box_z_end = (boundBox.z + coord_3d.z);

    rct_xy16 map = coordinate_3d_to_2d(&coord_3d, rotation);

    ps->x = map.x;
    ps->y = map.y;

    sint16 left = map.x + g1Element->x_offset;
    sint16 bottom = map.y + g1Element->y_offset;

    sint16 right = left + g1Element->width;
    sint16 top = bottom + g1Element->height;

    rct_drawpixelinfo *dpi = session->Unk140E9A8;

    if (right <= dpi->x) return NULL;
    if (top <= dpi->y) return NULL;
    if (left >= (dpi->x + dpi->width)) return NULL;
    if (bottom >= (dpi->y + dpi->height)) return NULL;

    ps->flags = 0;
    ps->bound_box_x = coord_3d.x;
    ps->bound_box_y = coord_3d.y;
    ps->attached_ps = NULL;
    ps->var_20 = NULL;
    ps->sprite_type = session->InteractionType;
    ps->var_29 = 0;
    ps->map_x = session->MapPosition.x;
    ps->map_y = session->MapPosition.y;
    ps->mapElement = session->CurrentlyDrawnItem;

    session->UnkF1AD28 = ps;

    sint32 positionHash = 0;
    switch (rotation) {
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
 * @param rotation (ebp)
 * @return (ebp) paint_struct on success (CF == 0), NULL on failure (CF == 1)
 */
paint_struct * sub_98197C(
    paint_session * session,
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
    uint32 rotation
) {
    session->UnkF1AD28 = 0;
    session->UnkF1AD2C = NULL;

    rct_xyz16 offset = {.x = x_offset, .y = y_offset, .z = z_offset};
    rct_xyz16 boundBoxSize = {.x = bound_box_length_x, .y = bound_box_length_y, .z = bound_box_length_z};
    rct_xyz16 boundBoxOffset = {.x = bound_box_offset_x, .y = bound_box_offset_y, .z = bound_box_offset_z};
    paint_struct * ps = sub_9819_c(session, image_id, offset, boundBoxSize, boundBoxOffset, rotation);

    if (ps == NULL) {
        return NULL;
    }

    session->UnkF1AD28 = ps;

    rct_xy16 attach = {
        .x = ps->bound_box_x,
        .y = ps->bound_box_y
    };

    rotate_map_coordinates(&attach.x, &attach.y, rotation);
    switch (rotation) {
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

    sint32 positionHash = attach.x + attach.y;
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
 * @param rotation (ebp)
 * @return (ebp) paint_struct on success (CF == 0), NULL on failure (CF == 1)
 */
paint_struct * sub_98198C(
    paint_session * session,
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
    uint32 rotation
) {
    assert((uint16) bound_box_length_x == (sint16) bound_box_length_x);
    assert((uint16) bound_box_length_y == (sint16) bound_box_length_y);

    session->UnkF1AD28 = 0;
    session->UnkF1AD2C = NULL;

    rct_xyz16 offset = {.x = x_offset, .y = y_offset, .z = z_offset};
    rct_xyz16 boundBoxSize = {.x = bound_box_length_x, .y = bound_box_length_y, .z = bound_box_length_z};
    rct_xyz16 boundBoxOffset = {.x = bound_box_offset_x, .y = bound_box_offset_y, .z = bound_box_offset_z};
    paint_struct * ps = sub_9819_c(session, image_id, offset, boundBoxSize, boundBoxOffset, rotation);

    if (ps == NULL) {
        return NULL;
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
 * @param rotation (ebp)
 * @return (ebp) paint_struct on success (CF == 0), NULL on failure (CF == 1)
 */
paint_struct * sub_98199C(
    paint_session * session,
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
    uint32 rotation
) {
    assert((uint16) bound_box_length_x == (sint16) bound_box_length_x);
    assert((uint16) bound_box_length_y == (sint16) bound_box_length_y);

    if (session->UnkF1AD28 == NULL) {
        return sub_98197C(session, 
            image_id,
            x_offset, y_offset,
            bound_box_length_x, bound_box_length_y, bound_box_length_z,
            z_offset,
            bound_box_offset_x, bound_box_offset_y, bound_box_offset_z,
            rotation
        );
    }

    rct_xyz16 offset = {.x = x_offset, .y = y_offset, .z = z_offset};
    rct_xyz16 boundBox = {.x = bound_box_length_x, .y = bound_box_length_y, .z = bound_box_length_z};
    rct_xyz16 boundBoxOffset = {.x = bound_box_offset_x, .y = bound_box_offset_y, .z = bound_box_offset_z};
    paint_struct * ps = sub_9819_c(session, image_id, offset, boundBox, boundBoxOffset, rotation);

    if (ps == NULL) {
        return NULL;
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
bool paint_attach_to_previous_attach(paint_session * session, uint32 image_id, uint16 x, uint16 y)
{
    if (session->UnkF1AD2C == NULL) {
        return paint_attach_to_previous_ps(session, image_id, x, y);
    }

    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray) {
        return false;
    }
    attached_paint_struct * ps = &session->NextFreePaintStruct->attached;
    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;

    attached_paint_struct * ebx = session->UnkF1AD2C;

    ps->next = NULL;
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
bool paint_attach_to_previous_ps(paint_session * session, uint32 image_id, uint16 x, uint16 y)
{
    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray) {
        return false;
    }
    attached_paint_struct * ps = &session->NextFreePaintStruct->attached;

    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;

    paint_struct * masterPs = session->UnkF1AD28;
    if (masterPs == NULL) {
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
void paint_floating_money_effect(paint_session * session, money32 amount, rct_string_id string_id, sint16 y, sint16 z, sint8 y_offsets[], sint16 offset_x, uint32 rotation)
{
    if (session->NextFreePaintStruct >= session->EndOfPaintStructArray) {
        return;
    }
    paint_string_struct * ps = &session->NextFreePaintStruct->string;

    ps->string_id = string_id;
    ps->next = 0;
    ps->args[0] = amount;
    ps->args[1] = y;
    ps->args[2] = 0;
    ps->args[3] = 0;
    ps->y_offsets = (uint8 *) y_offsets;

    rct_xyz16 position = {
        .x = session->SpritePosition.x,
        .y = session->SpritePosition.y,
        .z = z };
    rct_xy16 coord = coordinate_3d_to_2d(&position, rotation);

    ps->x = coord.x + offset_x;
    ps->y = coord.y;

    session->NextFreePaintStruct++;

    if (session->LastPSString == NULL) {
        session->PSStringHead = ps;
    } else {
        session->LastPSString->next = ps;
    }
    session->LastPSString = ps;
}

/**
 *
 *  rct2: 0x0068B6C2
 */
void paint_session_generate(paint_session * session)
{
    rct_drawpixelinfo * dpi = session->Unk140E9A8;
    rct_xy16 mapTile = {
        .x = dpi->x & 0xFFE0,
        .y = (dpi->y - 16) & 0xFFE0
    };

    sint16 half_x = mapTile.x >> 1;

    uint16 num_vertical_quadrants = (dpi->height + 2128) >> 5;

    switch (get_current_rotation()){
    case 0:
        mapTile.x = mapTile.y - half_x;
        mapTile.y = mapTile.y + half_x;

        mapTile.x &= 0xFFE0;
        mapTile.y &= 0xFFE0;

        for (; num_vertical_quadrants > 0; --num_vertical_quadrants){
            map_element_paint_setup(session, mapTile.x, mapTile.y);
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            sprite_paint_setup(session, mapTile.x - 32, mapTile.y + 32);

            map_element_paint_setup(session, mapTile.x, mapTile.y + 32);
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

        for (; num_vertical_quadrants > 0; --num_vertical_quadrants){
            map_element_paint_setup(session, mapTile.x, mapTile.y);
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            sprite_paint_setup(session, mapTile.x - 32, mapTile.y - 32);

            map_element_paint_setup(session, mapTile.x - 32, mapTile.y);
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

        for (; num_vertical_quadrants > 0; --num_vertical_quadrants){
            map_element_paint_setup(session, mapTile.x, mapTile.y);
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            sprite_paint_setup(session, mapTile.x + 32, mapTile.y - 32);

            map_element_paint_setup(session, mapTile.x, mapTile.y - 32);
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

        for (; num_vertical_quadrants > 0; --num_vertical_quadrants){
            map_element_paint_setup(session, mapTile.x, mapTile.y);
            sprite_paint_setup(session, mapTile.x, mapTile.y);

            sprite_paint_setup(session, mapTile.x + 32, mapTile.y + 32);

            map_element_paint_setup(session, mapTile.x + 32, mapTile.y);
            sprite_paint_setup(session, mapTile.x + 32, mapTile.y);

            mapTile.y -= 32;

            sprite_paint_setup(session, mapTile.x, mapTile.y);

            mapTile.x += 32;
        }
        break;
    }
}

static bool is_bbox_intersecting(uint8 rotation, const paint_struct_bound_box *initialBBox,
                                 const paint_struct_bound_box *currentBBox) {
    bool result = false;
    switch (rotation) {
    case 0:
        if ((*initialBBox).z_end >= (*currentBBox).z && (*initialBBox).y_end >= (*currentBBox).y && (*initialBBox).x_end >= (*currentBBox).x
            && !((*initialBBox).z < (*currentBBox).z_end && (*initialBBox).y < (*currentBBox).y_end && (*initialBBox).x < (*currentBBox).x_end))
            result = true;
        break;
    case 1:
        if ((*initialBBox).z_end >= (*currentBBox).z && (*initialBBox).y_end >= (*currentBBox).y && (*initialBBox).x_end < (*currentBBox).x
            && !((*initialBBox).z < (*currentBBox).z_end && (*initialBBox).y < (*currentBBox).y_end && (*initialBBox).x >= (*currentBBox).x_end))
            result = true;
        break;
    case 2:
        if ((*initialBBox).z_end >= (*currentBBox).z && (*initialBBox).y_end < (*currentBBox).y && (*initialBBox).x_end < (*currentBBox).x
            && !((*initialBBox).z < (*currentBBox).z_end && (*initialBBox).y >= (*currentBBox).y_end && (*initialBBox).x >= (*currentBBox).x_end))
            result = true;
        break;
    case 3:
        if ((*initialBBox).z_end >= (*currentBBox).z && (*initialBBox).y_end < (*currentBBox).y && (*initialBBox).x_end >= (*currentBBox).x
            && !((*initialBBox).z < (*currentBBox).z_end && (*initialBBox).y >= (*currentBBox).y_end && (*initialBBox).x < (*currentBBox).x_end))
            result = true;
        break;
    }
    return result;
}

paint_struct * paint_arrange_structs_helper(paint_struct * ps_next, uint16 quadrantIndex, uint8 flag)
{
    paint_struct * ps;
    paint_struct * ps_temp;
    do {
        ps = ps_next;
        ps_next = ps_next->next_quadrant_ps;
        if (ps_next == NULL) return ps;
    } while (quadrantIndex > ps_next->quadrant_index);

    // Cache the last visited node so we don't have to walk the whole list again
    paint_struct * ps_cache = ps;

    ps_temp = ps;
    do {
        ps = ps->next_quadrant_ps;
        if (ps == NULL) break;

        if (ps->quadrant_index > quadrantIndex + 1) {
            ps->quadrant_flags = PAINT_QUADRANT_FLAG_BIGGER;
        }
        else if (ps->quadrant_index == quadrantIndex + 1) {
            ps->quadrant_flags = PAINT_QUADRANT_FLAG_NEXT | PAINT_QUADRANT_FLAG_IDENTICAL;
        }
        else if (ps->quadrant_index == quadrantIndex) {
            ps->quadrant_flags = flag | PAINT_QUADRANT_FLAG_IDENTICAL;
        }
    } while (ps->quadrant_index <= quadrantIndex + 1);
    ps = ps_temp;

    uint8 rotation = get_current_rotation();
    while (true) {
        while (true) {
            ps_next = ps->next_quadrant_ps;
            if (ps_next == NULL) return ps_cache;
            if (ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_BIGGER) return ps_cache;
            if (ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_IDENTICAL) break;
            ps = ps_next;
        }

        ps_next->quadrant_flags &= ~PAINT_QUADRANT_FLAG_IDENTICAL;
        ps_temp = ps;

        const paint_struct_bound_box initialBBox = {
            .x = ps_next->bound_box_x,
            .y = ps_next->bound_box_y,
            .z = ps_next->bound_box_z,
            .x_end = ps_next->bound_box_x_end,
            .y_end = ps_next->bound_box_y_end,
            .z_end = ps_next->bound_box_z_end
        };


        while (true) {
            ps = ps_next;
            ps_next = ps_next->next_quadrant_ps;
            if (ps_next == NULL) break;
            if (ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_BIGGER) break;
            if (!(ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_NEXT)) continue;

            const paint_struct_bound_box currentBBox = {
                    .x = ps_next->bound_box_x,
                    .y = ps_next->bound_box_y,
                    .z = ps_next->bound_box_z,
                    .x_end = ps_next->bound_box_x_end,
                    .y_end = ps_next->bound_box_y_end,
                    .z_end = ps_next->bound_box_z_end
            };

            bool compareResult = is_bbox_intersecting(rotation, &initialBBox, &currentBBox);

            if (compareResult) {
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

/**
 *
 *  rct2: 0x00688217
 */
paint_struct paint_session_arrange(paint_session * session)
{
    paint_struct psHead = { 0 };
    paint_struct * ps = &psHead;
    ps->next_quadrant_ps = NULL;
    uint32 quadrantIndex = session->QuadrantBackIndex;
    if (quadrantIndex != UINT32_MAX) {
        do {
            paint_struct * ps_next = session->Quadrants[quadrantIndex];
            if (ps_next != NULL) {
                ps->next_quadrant_ps = ps_next;
                do {
                    ps = ps_next;
                    ps_next = ps_next->next_quadrant_ps;
                } while (ps_next != NULL);
            }
        } while (++quadrantIndex <= session->QuadrantFrontIndex);

        paint_struct * ps_cache = paint_arrange_structs_helper(&psHead, session->QuadrantBackIndex & 0xFFFF, PAINT_QUADRANT_FLAG_NEXT);

        quadrantIndex = session->QuadrantBackIndex;
        while (++quadrantIndex < session->QuadrantFrontIndex) {
            ps_cache = paint_arrange_structs_helper(ps_cache, quadrantIndex & 0xFFFF, 0);
        }
    }
    return psHead;
}

/**
 *
 *  rct2: 0x00688485
 */
void paint_draw_structs(rct_drawpixelinfo * dpi, paint_struct * ps, uint32 viewFlags)
{
    paint_struct* previous_ps = ps->next_quadrant_ps;
    for (ps = ps->next_quadrant_ps; ps;) {
        sint16 x = ps->x;
        sint16 y = ps->y;
        if (ps->sprite_type == VIEWPORT_INTERACTION_ITEM_SPRITE) {
            if (dpi->zoom_level >= 1) {
                x = floor2(x, 2);
                y = floor2(y, 2);
                if (dpi->zoom_level >= 2) {
                    x = floor2(x, 4);
                    y = floor2(y, 4);
                }
            }
        }

        uint32 imageId = paint_ps_colourify_image(ps->image_id, ps->sprite_type, viewFlags);
        if (gPaintBoundingBoxes && dpi->zoom_level == 0) {
            paint_ps_image_with_bounding_boxes(dpi, ps, imageId, x, y);
        } else {
            paint_ps_image(dpi, ps, imageId, x, y);
        }

        if (ps->var_20 != 0) {
            ps = ps->var_20;
        } else {
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
static void paint_attached_ps(rct_drawpixelinfo * dpi, paint_struct * ps, uint32 viewFlags)
{
    attached_paint_struct * attached_ps = ps->attached_ps;
    for (; attached_ps; attached_ps = attached_ps->next) {
        sint16 x = attached_ps->x + ps->x;
        sint16 y = attached_ps->y + ps->y;

        uint32 imageId = paint_ps_colourify_image(attached_ps->image_id, ps->sprite_type, viewFlags);
        if (attached_ps->flags & PAINT_STRUCT_FLAG_IS_MASKED) {
            gfx_draw_sprite_raw_masked(dpi, x, y, imageId, attached_ps->colour_image_id);
        } else {
            gfx_draw_sprite(dpi, imageId, x, y, ps->tertiary_colour);
        }
    }
}

static void paint_ps_image_with_bounding_boxes(rct_drawpixelinfo * dpi, paint_struct * ps, uint32 imageId, sint16 x, sint16 y)
{
    uint8 colour = BoundBoxDebugColours[ps->sprite_type];

    rct_xyz16 frontTop = {.x = ps->bound_box_x_end, .y = ps->bound_box_y_end, .z = ps->bound_box_z_end};
    rct_xy16 screenCoordFrontTop = coordinate_3d_to_2d(&frontTop, get_current_rotation());
    rct_xyz16 frontBottom = {.x = ps->bound_box_x_end, .y = ps->bound_box_y_end, .z = ps->bound_box_z};
    rct_xy16 screenCoordFrontBottom = coordinate_3d_to_2d(&frontBottom, get_current_rotation());

    rct_xyz16 leftTop = {.x = ps->bound_box_x, .y = ps->bound_box_y_end, .z = ps->bound_box_z_end};
    rct_xy16 screenCoordLeftTop = coordinate_3d_to_2d(&leftTop, get_current_rotation());
    rct_xyz16 leftBottom = {.x = ps->bound_box_x, .y = ps->bound_box_y_end, .z = ps->bound_box_z};
    rct_xy16 screenCoordLeftBottom = coordinate_3d_to_2d(&leftBottom, get_current_rotation());

    rct_xyz16 rightTop = {.x = ps->bound_box_x_end, .y = ps->bound_box_y, .z = ps->bound_box_z_end};
    rct_xy16 screenCoordRightTop = coordinate_3d_to_2d(&rightTop, get_current_rotation());
    rct_xyz16 rightBottom = {.x = ps->bound_box_x_end, .y = ps->bound_box_y, .z = ps->bound_box_z};
    rct_xy16 screenCoordRightBottom = coordinate_3d_to_2d(&rightBottom, get_current_rotation());

    rct_xyz16 backTop = {.x = ps->bound_box_x, .y = ps->bound_box_y, .z = ps->bound_box_z_end};
    rct_xy16 screenCoordBackTop = coordinate_3d_to_2d(&backTop, get_current_rotation());
    rct_xyz16 backBottom = {.x = ps->bound_box_x, .y = ps->bound_box_y, .z = ps->bound_box_z};
    rct_xy16 screenCoordBackBottom = coordinate_3d_to_2d(&backBottom, get_current_rotation());

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

static void paint_ps_image(rct_drawpixelinfo * dpi, paint_struct * ps, uint32 imageId, sint16 x, sint16 y)
{
    if (ps->flags & PAINT_STRUCT_FLAG_IS_MASKED) {
        gfx_draw_sprite_raw_masked(dpi, x, y, imageId, ps->colour_image_id);
    } else {
        gfx_draw_sprite(dpi, imageId, x, y, ps->tertiary_colour);
    }
}

static uint32 paint_ps_colourify_image(uint32 imageId, uint8 spriteType, uint32 viewFlags)
{
    if (viewFlags & VIEWPORT_FLAG_SEETHROUGH_RIDES) {
        if (spriteType == VIEWPORT_INTERACTION_ITEM_RIDE) {
            if (!(imageId & IMAGE_TYPE_TRANSPARENT)) {
                imageId &= 0x7FFFF;
                imageId |= 0x41880000;
            }
        }
    }
    if (viewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE) {
        if (spriteType == VIEWPORT_INTERACTION_ITEM_WALL) {
            if (!(imageId & IMAGE_TYPE_TRANSPARENT)) {
                imageId &= 0x7FFFF;
                imageId |= 0x41880000;
            }
        }
    }
    if (viewFlags & VIEWPORT_FLAG_SEETHROUGH_PATHS) {
        switch (spriteType) {
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
        case VIEWPORT_INTERACTION_ITEM_BANNER:
            if (!(imageId & IMAGE_TYPE_TRANSPARENT)) {
                imageId &= 0x7FFFF;
                imageId |= 0x41880000;
            }
            break;
        }
    }
    if (viewFlags & VIEWPORT_FLAG_SEETHROUGH_SCENERY) {
        switch (spriteType) {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        case VIEWPORT_INTERACTION_ITEM_WALL:
            if (!(imageId & IMAGE_TYPE_TRANSPARENT)) {
                imageId &= 0x7FFFF;
                imageId |= 0x41880000;
            }
            break;
        }
    }
    return imageId;
}

static void draw_pixel_info_crop_by_zoom(rct_drawpixelinfo *dpi)
{
    sint32 zoom = dpi->zoom_level;
    dpi->zoom_level = 0;
    dpi->x >>= zoom;
    dpi->y >>= zoom;
    dpi->width >>= zoom;
    dpi->height >>= zoom;
}

/**
 *
 *  rct2: 0x006860C3
 */
void paint_draw_money_structs(rct_drawpixelinfo * dpi, paint_string_struct * ps)
{
    rct_drawpixelinfo dpi2 = *dpi;
    draw_pixel_info_crop_by_zoom(&dpi2);

    do {
        utf8 buffer[256];
        format_string(buffer, 256, ps->string_id, &ps->args);
        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

        // Use sprite font unless the currency contains characters unsupported by the sprite font
        bool forceSpriteFont = false;
        const currency_descriptor *currencyDesc = &CurrencyDescriptors[gConfigGeneral.currency_format];
        if (gUseTrueTypeFont && font_supports_string_sprite(currencyDesc->symbol_unicode)) {
            forceSpriteFont = true;
        }

        gfx_draw_string_with_y_offsets(&dpi2, buffer, COLOUR_BLACK, ps->x, ps->y, (sint8 *)ps->y_offsets, forceSpriteFont);
    } while ((ps = ps->next) != NULL);
}
