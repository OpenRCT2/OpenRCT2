/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.h"

#include "../Context.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../localisation/Localisation.h"
#include "../localisation/LocalisationService.h"
#include "../paint/Painter.h"
#include "sprite/Paint.Sprite.h"
#include "tile_element/Paint.TileElement.h"

#include <algorithm>
#include <array>
#include <atomic>

using namespace OpenRCT2;

// Globals for paint clipping
uint8_t gClipHeight = 128; // Default to middle value
CoordsXY gClipSelectionA = { 0, 0 };
CoordsXY gClipSelectionB = { MAXIMUM_TILE_START_XY, MAXIMUM_TILE_START_XY };

static constexpr const uint8_t BoundBoxDebugColours[] = {
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

static void PaintAttachedPS(rct_drawpixelinfo* dpi, paint_struct* ps, uint32_t viewFlags);
static void PaintPSImageWithBoundingBoxes(rct_drawpixelinfo* dpi, paint_struct* ps, uint32_t imageId, int16_t x, int16_t y);
static void PaintPSImage(rct_drawpixelinfo* dpi, paint_struct* ps, uint32_t imageId, int16_t x, int16_t y);
static uint32_t PaintPSColourifyImage(uint32_t imageId, ViewportInteractionItem spriteType, uint32_t viewFlags);

static constexpr int32_t CalculatePositionHash(const paint_struct& ps, uint8_t rotation)
{
    auto pos = CoordsXY{ static_cast<int16_t>(ps.bounds.x), static_cast<int16_t>(ps.bounds.y) }.Rotate(rotation);
    switch (rotation)
    {
        case 0:
            break;
        case 1:
        case 3:
            pos.x += 0x2000;
            break;
        case 2:
            pos.x += 0x4000;
            break;
    }

    return pos.x + pos.y;
}

static void PaintSessionAddPSToQuadrant(paint_session* session, paint_struct* ps)
{
    auto positionHash = CalculatePositionHash(*ps, session->CurrentRotation);
    uint32_t paintQuadrantIndex = std::clamp(positionHash / 32, 0, MAX_PAINT_QUADRANTS - 1);
    ps->quadrant_index = paintQuadrantIndex;
    ps->next_quadrant_ps = session->Quadrants[paintQuadrantIndex];
    session->Quadrants[paintQuadrantIndex] = ps;

    session->QuadrantBackIndex = std::min(session->QuadrantBackIndex, paintQuadrantIndex);
    session->QuadrantFrontIndex = std::max(session->QuadrantFrontIndex, paintQuadrantIndex);
}

static constexpr bool ImageWithinDPI(const ScreenCoordsXY& imagePos, const rct_g1_element& g1, const rct_drawpixelinfo& dpi)
{
    int32_t left = imagePos.x + g1.x_offset;
    int32_t bottom = imagePos.y + g1.y_offset;

    int32_t right = left + g1.width;
    int32_t top = bottom + g1.height;

    if (right <= dpi.x)
        return false;
    if (top <= dpi.y)
        return false;
    if (left >= dpi.x + dpi.width)
        return false;
    if (bottom >= dpi.y + dpi.height)
        return false;
    return true;
}

static constexpr CoordsXYZ RotateBoundBoxSize(const CoordsXYZ& bbSize, const uint8_t rotation)
{
    auto output = bbSize;
    // This probably rotates the variables so they're relative to rotation 0.
    switch (rotation)
    {
        case 0:
            output.x--;
            output.y--;
            output = { output.Rotate(0), output.z };
            break;
        case 1:
            output.x--;
            output = { output.Rotate(3), output.z };
            break;
        case 2:
            output = { output.Rotate(2), output.z };
            break;
        case 3:
            output.y--;
            output = { output.Rotate(1), output.z };
            break;
    }
    return output;
}

/**
 * Extracted from 0x0098196c, 0x0098197c, 0x0098198c, 0x0098199c
 */
static paint_struct* CreateNormalPaintStruct(
    paint_session* session, const uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize,
    const CoordsXYZ& boundBoxOffset)
{
    if (session->NoPaintStructsAvailable())
        return nullptr;

    auto* const g1 = gfx_get_g1_element(image_id & 0x7FFFF);
    if (g1 == nullptr)
    {
        return nullptr;
    }

    const uint8_t swappedRotation = (session->CurrentRotation * 3) % 4; // swaps 1 and 3
    auto swappedRotCoord = CoordsXYZ{ offset.Rotate(swappedRotation), offset.z };
    swappedRotCoord += session->SpritePosition;

    const auto imagePos = translate_3d_to_2d_with_z(session->CurrentRotation, swappedRotCoord);

    if (!ImageWithinDPI(imagePos, *g1, session->DPI))
    {
        return nullptr;
    }

    const auto rotBoundBoxOffset = CoordsXYZ{ boundBoxOffset.Rotate(swappedRotation), boundBoxOffset.z };
    const auto rotBoundBoxSize = RotateBoundBoxSize(boundBoxSize, session->CurrentRotation);

    paint_struct* ps = session->AllocateNormalPaintEntry();
    ps->image_id = image_id;
    ps->x = imagePos.x;
    ps->y = imagePos.y;
    ps->bounds.x_end = rotBoundBoxSize.x + rotBoundBoxOffset.x + session->SpritePosition.x;
    ps->bounds.y_end = rotBoundBoxSize.y + rotBoundBoxOffset.y + session->SpritePosition.y;
    ps->bounds.z_end = rotBoundBoxSize.z + rotBoundBoxOffset.z;
    ps->bounds.x = rotBoundBoxOffset.x + session->SpritePosition.x;
    ps->bounds.y = rotBoundBoxOffset.y + session->SpritePosition.y;
    ps->bounds.z = rotBoundBoxOffset.z;
    ps->flags = 0;
    ps->attached_ps = nullptr;
    ps->children = nullptr;
    ps->sprite_type = session->InteractionType;
    ps->var_29 = 0;
    ps->map_x = session->MapPosition.x;
    ps->map_y = session->MapPosition.y;
    ps->tileElement = reinterpret_cast<TileElement*>(const_cast<void*>(session->CurrentlyDrawnItem));

    return ps;
}

template<uint8_t direction> void PaintSessionGenerateRotate(paint_session* session)
{
    // Optimised modified version of viewport_coord_to_map_coord
    ScreenCoordsXY screenCoord = { static_cast<int16_t>((session->DPI.x) & 0xFFE0),
                                   static_cast<int16_t>((session->DPI.y - 16) & 0xFFE0) };
    CoordsXY mapTile = { screenCoord.y - screenCoord.x / 2, screenCoord.y + screenCoord.x / 2 };
    mapTile = mapTile.Rotate(direction);

    if constexpr (direction & 1)
    {
        mapTile.y -= 16;
    }
    mapTile = mapTile.ToTileStart();

    uint16_t numVerticalTiles = (session->DPI.height + 2128) >> 5;

    // Adjacent tiles to also check due to overlapping of sprites
    constexpr CoordsXY adjacentTiles[] = { CoordsXY{ -32, 32 }.Rotate(direction), CoordsXY{ 0, 32 }.Rotate(direction),
                                           CoordsXY{ 32, 0 }.Rotate(direction) };
    constexpr CoordsXY nextVerticalTile = CoordsXY{ 32, 32 }.Rotate(direction);

    for (; numVerticalTiles > 0; --numVerticalTiles)
    {
        tile_element_paint_setup(session, mapTile.x, mapTile.y);
        sprite_paint_setup(session, mapTile.x, mapTile.y);

        auto loc1 = mapTile + adjacentTiles[0];
        sprite_paint_setup(session, loc1.x, loc1.y);

        auto loc2 = mapTile + adjacentTiles[1];
        tile_element_paint_setup(session, loc2.x, loc2.y);
        sprite_paint_setup(session, loc2.x, loc2.y);

        auto loc3 = mapTile + adjacentTiles[2];
        sprite_paint_setup(session, loc3.x, loc3.y);

        mapTile += nextVerticalTile;
    }
}

/**
 *
 *  rct2: 0x0068B6C2
 */
void PaintSessionGenerate(paint_session* session)
{
    session->CurrentRotation = get_current_rotation();

    // Extracted from viewport_coord_to_map_coord
    constexpr uint8_t inverseRotationMapping[NumOrthogonalDirections] = { 0, 3, 2, 1 };
    switch (inverseRotationMapping[session->CurrentRotation])
    {
        case 0:
            PaintSessionGenerateRotate<0>(session);
            break;
        case 1:
            PaintSessionGenerateRotate<1>(session);
            break;
        case 2:
            PaintSessionGenerateRotate<2>(session);
            break;
        case 3:
            PaintSessionGenerateRotate<3>(session);
            break;
    }
}

template<uint8_t>
static bool CheckBoundingBox(const paint_struct_bound_box& initialBBox, const paint_struct_bound_box& currentBBox)
{
    return false;
}

template<> bool CheckBoundingBox<0>(const paint_struct_bound_box& initialBBox, const paint_struct_bound_box& currentBBox)
{
    if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end >= currentBBox.y && initialBBox.x_end >= currentBBox.x
        && !(initialBBox.z < currentBBox.z_end && initialBBox.y < currentBBox.y_end && initialBBox.x < currentBBox.x_end))
    {
        return true;
    }
    return false;
}

template<> bool CheckBoundingBox<1>(const paint_struct_bound_box& initialBBox, const paint_struct_bound_box& currentBBox)
{
    if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end >= currentBBox.y && initialBBox.x_end < currentBBox.x
        && !(initialBBox.z < currentBBox.z_end && initialBBox.y < currentBBox.y_end && initialBBox.x >= currentBBox.x_end))
    {
        return true;
    }
    return false;
}

template<> bool CheckBoundingBox<2>(const paint_struct_bound_box& initialBBox, const paint_struct_bound_box& currentBBox)
{
    if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end < currentBBox.y && initialBBox.x_end < currentBBox.x
        && !(initialBBox.z < currentBBox.z_end && initialBBox.y >= currentBBox.y_end && initialBBox.x >= currentBBox.x_end))
    {
        return true;
    }
    return false;
}

template<> bool CheckBoundingBox<3>(const paint_struct_bound_box& initialBBox, const paint_struct_bound_box& currentBBox)
{
    if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end < currentBBox.y && initialBBox.x_end >= currentBBox.x
        && !(initialBBox.z < currentBBox.z_end && initialBBox.y >= currentBBox.y_end && initialBBox.x < currentBBox.x_end))
    {
        return true;
    }
    return false;
}

template<uint8_t _TRotation>
static paint_struct* PaintArrangeStructsHelperRotation(paint_struct* ps_next, uint16_t quadrantIndex, uint8_t flag)
{
    paint_struct* ps;
    paint_struct* ps_temp;
    do
    {
        ps = ps_next;
        ps_next = ps_next->next_quadrant_ps;
        if (ps_next == nullptr)
            return ps;
    } while (quadrantIndex > ps_next->quadrant_index);

    // Cache the last visited node so we don't have to walk the whole list again
    paint_struct* ps_cache = ps;

    ps_temp = ps;
    do
    {
        ps = ps->next_quadrant_ps;
        if (ps == nullptr)
            break;

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
            if (ps_next == nullptr)
                return ps_cache;
            if (ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_BIGGER)
                return ps_cache;
            if (ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_IDENTICAL)
                break;
            ps = ps_next;
        }

        ps_next->quadrant_flags &= ~PAINT_QUADRANT_FLAG_IDENTICAL;
        ps_temp = ps;

        const paint_struct_bound_box& initialBBox = ps_next->bounds;

        while (true)
        {
            ps = ps_next;
            ps_next = ps_next->next_quadrant_ps;
            if (ps_next == nullptr)
                break;
            if (ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_BIGGER)
                break;
            if (!(ps_next->quadrant_flags & PAINT_QUADRANT_FLAG_NEXT))
                continue;

            const paint_struct_bound_box& currentBBox = ps_next->bounds;

            const bool compareResult = CheckBoundingBox<_TRotation>(initialBBox, currentBBox);

            if (compareResult)
            {
                ps->next_quadrant_ps = ps_next->next_quadrant_ps;
                paint_struct* ps_temp2 = ps_temp->next_quadrant_ps;
                ps_temp->next_quadrant_ps = ps_next;
                ps_next->next_quadrant_ps = ps_temp2;
                ps_next = ps;
            }
        }

        ps = ps_temp;
    }
}

template<int TRotation> static void PaintSessionArrange(paint_session* session, bool)
{
    paint_struct* psHead = &session->PaintHead;

    paint_struct* ps = psHead;
    ps->next_quadrant_ps = nullptr;

    uint32_t quadrantIndex = session->QuadrantBackIndex;
    if (quadrantIndex != UINT32_MAX)
    {
        do
        {
            paint_struct* ps_next = session->Quadrants[quadrantIndex];
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

        paint_struct* ps_cache = PaintArrangeStructsHelperRotation<TRotation>(
            psHead, session->QuadrantBackIndex & 0xFFFF, PAINT_QUADRANT_FLAG_NEXT);

        quadrantIndex = session->QuadrantBackIndex;
        while (++quadrantIndex < session->QuadrantFrontIndex)
        {
            ps_cache = PaintArrangeStructsHelperRotation<TRotation>(ps_cache, quadrantIndex & 0xFFFF, 0);
        }
    }
}

/**
 *
 *  rct2: 0x00688217
 */
void PaintSessionArrange(paint_session* session)
{
    switch (session->CurrentRotation)
    {
        case 0:
            return PaintSessionArrange<0>(session, true);
        case 1:
            return PaintSessionArrange<1>(session, true);
        case 2:
            return PaintSessionArrange<2>(session, true);
        case 3:
            return PaintSessionArrange<3>(session, true);
    }
    Guard::Assert(false);
}

static void PaintDrawStruct(paint_session* session, paint_struct* ps)
{
    rct_drawpixelinfo* dpi = &session->DPI;

    int16_t x = ps->x;
    int16_t y = ps->y;

    if (ps->sprite_type == ViewportInteractionItem::Entity)
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

    uint32_t imageId = PaintPSColourifyImage(ps->image_id, ps->sprite_type, session->ViewFlags);
    if (gPaintBoundingBoxes && dpi->zoom_level == 0)
    {
        PaintPSImageWithBoundingBoxes(dpi, ps, imageId, x, y);
    }
    else
    {
        PaintPSImage(dpi, ps, imageId, x, y);
    }

    if (ps->children != nullptr)
    {
        PaintDrawStruct(session, ps->children);
    }
    else
    {
        PaintAttachedPS(dpi, ps, session->ViewFlags);
    }
}

/**
 *
 *  rct2: 0x00688485
 */
void PaintDrawStructs(paint_session* session)
{
    paint_struct* ps = &session->PaintHead;

    for (ps = ps->next_quadrant_ps; ps;)
    {
        PaintDrawStruct(session, ps);

        ps = ps->next_quadrant_ps;
    }
}

/**
 *
 *  rct2: 0x00688596
 *  Part of 0x688485
 */
static void PaintAttachedPS(rct_drawpixelinfo* dpi, paint_struct* ps, uint32_t viewFlags)
{
    attached_paint_struct* attached_ps = ps->attached_ps;
    for (; attached_ps; attached_ps = attached_ps->next)
    {
        auto screenCoords = ScreenCoordsXY{ attached_ps->x + ps->x, attached_ps->y + ps->y };

        uint32_t imageId = PaintPSColourifyImage(attached_ps->image_id, ps->sprite_type, viewFlags);
        if (attached_ps->flags & PAINT_STRUCT_FLAG_IS_MASKED)
        {
            gfx_draw_sprite_raw_masked(dpi, screenCoords, imageId, attached_ps->colour_image_id);
        }
        else
        {
            gfx_draw_sprite(dpi, imageId, screenCoords, ps->tertiary_colour);
        }
    }
}

static void PaintPSImageWithBoundingBoxes(rct_drawpixelinfo* dpi, paint_struct* ps, uint32_t imageId, int16_t x, int16_t y)
{
    const uint8_t colour = BoundBoxDebugColours[EnumValue(ps->sprite_type)];
    const uint8_t rotation = get_current_rotation();

    const CoordsXYZ frontTop = {
        ps->bounds.x_end,
        ps->bounds.y_end,
        ps->bounds.z_end,
    };
    const auto screenCoordFrontTop = translate_3d_to_2d_with_z(rotation, frontTop);

    const CoordsXYZ frontBottom = {
        ps->bounds.x_end,
        ps->bounds.y_end,
        ps->bounds.z,
    };
    const auto screenCoordFrontBottom = translate_3d_to_2d_with_z(rotation, frontBottom);

    const CoordsXYZ leftTop = {
        ps->bounds.x,
        ps->bounds.y_end,
        ps->bounds.z_end,
    };
    const auto screenCoordLeftTop = translate_3d_to_2d_with_z(rotation, leftTop);

    const CoordsXYZ leftBottom = {
        ps->bounds.x,
        ps->bounds.y_end,
        ps->bounds.z,
    };
    const auto screenCoordLeftBottom = translate_3d_to_2d_with_z(rotation, leftBottom);

    const CoordsXYZ rightTop = {
        ps->bounds.x_end,
        ps->bounds.y,
        ps->bounds.z_end,
    };
    const auto screenCoordRightTop = translate_3d_to_2d_with_z(rotation, rightTop);

    const CoordsXYZ rightBottom = {
        ps->bounds.x_end,
        ps->bounds.y,
        ps->bounds.z,
    };
    const auto screenCoordRightBottom = translate_3d_to_2d_with_z(rotation, rightBottom);

    const CoordsXYZ backTop = {
        ps->bounds.x,
        ps->bounds.y,
        ps->bounds.z_end,
    };
    const auto screenCoordBackTop = translate_3d_to_2d_with_z(rotation, backTop);

    const CoordsXYZ backBottom = {
        ps->bounds.x,
        ps->bounds.y,
        ps->bounds.z,
    };
    const auto screenCoordBackBottom = translate_3d_to_2d_with_z(rotation, backBottom);

    // bottom square
    gfx_draw_line(dpi, { screenCoordFrontBottom, screenCoordLeftBottom }, colour);
    gfx_draw_line(dpi, { screenCoordBackBottom, screenCoordLeftBottom }, colour);
    gfx_draw_line(dpi, { screenCoordBackBottom, screenCoordRightBottom }, colour);
    gfx_draw_line(dpi, { screenCoordFrontBottom, screenCoordRightBottom }, colour);

    // vertical back + sides
    gfx_draw_line(dpi, { screenCoordBackTop, screenCoordBackBottom }, colour);
    gfx_draw_line(dpi, { screenCoordLeftTop, screenCoordLeftBottom }, colour);
    gfx_draw_line(dpi, { screenCoordRightTop, screenCoordRightBottom }, colour);

    // top square back
    gfx_draw_line(dpi, { screenCoordBackTop, screenCoordLeftTop }, colour);
    gfx_draw_line(dpi, { screenCoordBackTop, screenCoordRightTop }, colour);

    PaintPSImage(dpi, ps, imageId, x, y);

    // vertical front
    gfx_draw_line(dpi, { screenCoordFrontTop, screenCoordFrontBottom }, colour);

    // top square
    gfx_draw_line(dpi, { screenCoordFrontTop, screenCoordLeftTop }, colour);
    gfx_draw_line(dpi, { screenCoordFrontTop, screenCoordRightTop }, colour);
}

static void PaintPSImage(rct_drawpixelinfo* dpi, paint_struct* ps, uint32_t imageId, int16_t x, int16_t y)
{
    if (ps->flags & PAINT_STRUCT_FLAG_IS_MASKED)
    {
        return gfx_draw_sprite_raw_masked(dpi, { x, y }, imageId, ps->colour_image_id);
    }

    gfx_draw_sprite(dpi, imageId, { x, y }, ps->tertiary_colour);
}

static uint32_t PaintPSColourifyImage(uint32_t imageId, ViewportInteractionItem spriteType, uint32_t viewFlags)
{
    constexpr uint32_t primaryColour = COLOUR_BRIGHT_YELLOW;
    constexpr uint32_t secondaryColour = COLOUR_GREY;
    constexpr uint32_t seeThoughFlags = IMAGE_TYPE_TRANSPARENT | (primaryColour << 19) | (secondaryColour << 24);

    if (viewFlags & VIEWPORT_FLAG_SEETHROUGH_RIDES)
    {
        if (spriteType == ViewportInteractionItem::Ride)
        {
            imageId &= 0x7FFFF;
            imageId |= seeThoughFlags;
        }
    }
    if (viewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
    {
        if (spriteType == ViewportInteractionItem::Wall)
        {
            imageId &= 0x7FFFF;
            imageId |= seeThoughFlags;
        }
    }
    if (viewFlags & VIEWPORT_FLAG_SEETHROUGH_PATHS)
    {
        switch (spriteType)
        {
            case ViewportInteractionItem::Footpath:
            case ViewportInteractionItem::FootpathItem:
            case ViewportInteractionItem::Banner:
                imageId &= 0x7FFFF;
                imageId |= seeThoughFlags;
            default:
                break;
        }
    }
    if (viewFlags & VIEWPORT_FLAG_SEETHROUGH_SCENERY)
    {
        switch (spriteType)
        {
            case ViewportInteractionItem::Scenery:
            case ViewportInteractionItem::LargeScenery:
            case ViewportInteractionItem::Wall:
                imageId &= 0x7FFFF;
                imageId |= seeThoughFlags;
            default:
                break;
        }
    }
    return imageId;
}

paint_session* PaintSessionAlloc(rct_drawpixelinfo* dpi, uint32_t viewFlags)
{
    return GetContext()->GetPainter()->CreateSession(dpi, viewFlags);
}

void PaintSessionFree([[maybe_unused]] paint_session* session)
{
    GetContext()->GetPainter()->ReleaseSession(session);
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
paint_struct* PaintAddImageAsParent(
    paint_session* session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize)
{
    return PaintAddImageAsParent(
        session, image_id, offset.x, offset.y, boundBoxSize.x, boundBoxSize.y, boundBoxSize.z, offset.z, offset.x, offset.y,
        offset.z);
}

paint_struct* PaintAddImageAsParent(
    paint_session* session, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset)
{
    return PaintAddImageAsParent(
        session, image_id, { x_offset, y_offset, z_offset }, { bound_box_length_x, bound_box_length_y, bound_box_length_z });
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
paint_struct* PaintAddImageAsParent(
    paint_session* session, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x,
    int16_t bound_box_offset_y, int16_t bound_box_offset_z)
{
    session->LastPS = nullptr;
    session->LastAttachedPS = nullptr;

    CoordsXYZ offset = { x_offset, y_offset, z_offset };
    CoordsXYZ boundBoxSize = { bound_box_length_x, bound_box_length_y, bound_box_length_z };
    CoordsXYZ boundBoxOffset = { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z };

    auto* ps = CreateNormalPaintStruct(session, image_id, offset, boundBoxSize, boundBoxOffset);
    if (ps == nullptr)
    {
        return nullptr;
    }

    PaintSessionAddPSToQuadrant(session, ps);

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
 * Creates a paint struct but does not allocate to a paint quadrant. Result cannot be ignored!
 */
[[nodiscard]] paint_struct* PaintAddImageAsOrphan(
    paint_session* session, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x,
    int16_t bound_box_offset_y, int16_t bound_box_offset_z)
{
    assert(static_cast<uint16_t>(bound_box_length_x) == static_cast<int16_t>(bound_box_length_x));
    assert(static_cast<uint16_t>(bound_box_length_y) == static_cast<int16_t>(bound_box_length_y));

    session->LastPS = nullptr;
    session->LastAttachedPS = nullptr;

    CoordsXYZ offset = { x_offset, y_offset, z_offset };
    CoordsXYZ boundBoxSize = { bound_box_length_x, bound_box_length_y, bound_box_length_z };
    CoordsXYZ boundBoxOffset = { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z };

    auto* ps = CreateNormalPaintStruct(session, image_id, offset, boundBoxSize, boundBoxOffset);
    if (ps == nullptr)
    {
        return nullptr;
    }

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
 * If there is no parent paint struct then image is added as a parent
 */
paint_struct* PaintAddImageAsChild(
    paint_session* session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxLength,
    const CoordsXYZ& boundBoxOffset)
{
    paint_struct* parentPS = session->LastPS;
    if (parentPS == nullptr)
    {
        return PaintAddImageAsParent(
            session, image_id, offset.x, offset.y, boundBoxLength.x, boundBoxLength.y, boundBoxLength.z, offset.z,
            boundBoxOffset.x, boundBoxOffset.y, boundBoxOffset.z);
    }

    auto* ps = CreateNormalPaintStruct(session, image_id, offset, boundBoxLength, boundBoxOffset);
    if (ps == nullptr)
    {
        return nullptr;
    }

    parentPS->children = ps;

    return ps;
}

paint_struct* PaintAddImageAsChild(
    paint_session* session, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x,
    int16_t bound_box_offset_y, int16_t bound_box_offset_z)
{
    assert(static_cast<uint16_t>(bound_box_length_x) == static_cast<int16_t>(bound_box_length_x));
    assert(static_cast<uint16_t>(bound_box_length_y) == static_cast<int16_t>(bound_box_length_y));
    return PaintAddImageAsChild(
        session, image_id, { x_offset, y_offset, z_offset }, { bound_box_length_x, bound_box_length_y, bound_box_length_z },
        { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z });
}

/**
 * rct2: 0x006881D0
 *
 * @param image_id (ebx)
 * @param x (ax)
 * @param y (cx)
 * @return (!CF) success
 */
bool PaintAttachToPreviousAttach(paint_session* session, uint32_t image_id, int16_t x, int16_t y)
{
    if (session->NoPaintStructsAvailable())
    {
        return false;
    }

    attached_paint_struct* previousAttachedPS = session->LastAttachedPS;
    if (previousAttachedPS == nullptr)
    {
        return PaintAttachToPreviousPS(session, image_id, x, y);
    }

    attached_paint_struct* ps = session->AllocateAttachedPaintEntry();
    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;
    ps->next = nullptr;

    previousAttachedPS->next = ps;

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
bool PaintAttachToPreviousPS(paint_session* session, uint32_t image_id, int16_t x, int16_t y)
{
    if (session->NoPaintStructsAvailable())
    {
        return false;
    }

    paint_struct* masterPs = session->LastPS;
    if (masterPs == nullptr)
    {
        return false;
    }

    attached_paint_struct* ps = session->AllocateAttachedPaintEntry();
    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;

    attached_paint_struct* oldFirstAttached = masterPs->attached_ps;
    masterPs->attached_ps = ps;
    ps->next = oldFirstAttached;

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
void PaintFloatingMoneyEffect(
    paint_session* session, money32 amount, rct_string_id string_id, int16_t y, int16_t z, int8_t y_offsets[], int16_t offset_x,
    uint32_t rotation)
{
    if (session->NoPaintStructsAvailable())
    {
        return;
    }

    const CoordsXYZ position = {
        session->SpritePosition.x,
        session->SpritePosition.y,
        z,
    };
    const auto coord = translate_3d_to_2d_with_z(rotation, position);

    paint_string_struct* ps = session->AllocateStringPaintEntry();
    ps->string_id = string_id;
    ps->next = nullptr;
    ps->args[0] = amount;
    ps->args[1] = y;
    ps->args[2] = 0;
    ps->args[3] = 0;
    ps->y_offsets = reinterpret_cast<uint8_t*>(y_offsets);
    ps->x = coord.x + offset_x;
    ps->y = coord.y;
}

/**
 *
 *  rct2: 0x006860C3
 */
void PaintDrawMoneyStructs(rct_drawpixelinfo* dpi, paint_string_struct* ps)
{
    do
    {
        char buffer[256]{};
        format_string(buffer, sizeof(buffer), ps->string_id, &ps->args);

        // Use sprite font unless the currency contains characters unsupported by the sprite font
        auto forceSpriteFont = false;
        const auto& currencyDesc = CurrencyDescriptors[EnumValue(gConfigGeneral.currency_format)];
        if (LocalisationService_UseTrueTypeFont() && font_supports_string_sprite(currencyDesc.symbol_unicode))
        {
            forceSpriteFont = true;
        }

        gfx_draw_string_with_y_offsets(
            dpi, buffer, COLOUR_BLACK, { ps->x, ps->y }, reinterpret_cast<int8_t*>(ps->y_offsets), forceSpriteFont,
            FontSpriteBase::MEDIUM);
    } while ((ps = ps->next) != nullptr);
}
