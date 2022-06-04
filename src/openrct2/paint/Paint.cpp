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
#include "../profiling/Profiling.h"
#include "../util/Math.hpp"
#include "../world/SmallScenery.h"
#include "Paint.Entity.h"
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
static void PaintPSImageWithBoundingBoxes(rct_drawpixelinfo* dpi, paint_struct* ps, ImageId imageId, int32_t x, int32_t y);
static ImageId PaintPSColourifyImage(const paint_struct* ps, ImageId imageId, uint32_t viewFlags);

static int32_t RemapPositionToQuadrant(const paint_struct& ps, uint8_t rotation)
{
    constexpr auto MapRangeMax = MaxPaintQuadrants * COORDS_XY_STEP;
    constexpr auto MapRangeCenter = MapRangeMax / 2;

    const auto x = ps.bounds.x;
    const auto y = ps.bounds.y;
    // NOTE: We are not calling CoordsXY::Rotate on purpose to mix in the additional
    // value without a secondary switch.
    switch (rotation & 3)
    {
        case 0:
            return x + y;
        case 1:
            // Because one component may be the maximum we add the center to be a positive value.
            return (y - x) + MapRangeCenter;
        case 2:
            // If both components would be the maximum it would be the negative xy, to be positive add max.
            return (-(y + x)) + MapRangeMax;
        case 3:
            // Same as 1 but inverted.
            return (x - y) + MapRangeCenter;
    }
    return 0;
}

static void PaintSessionAddPSToQuadrant(paint_session& session, paint_struct* ps)
{
    const auto positionHash = RemapPositionToQuadrant(*ps, session.CurrentRotation);

    // Values below zero or above MaxPaintQuadrants are void, corners also share the same quadrant as void.
    const uint32_t paintQuadrantIndex = std::clamp(positionHash / COORDS_XY_STEP, 0, MaxPaintQuadrants - 1);

    ps->quadrant_index = paintQuadrantIndex;
    ps->next_quadrant_ps = session.Quadrants[paintQuadrantIndex];
    session.Quadrants[paintQuadrantIndex] = ps;

    session.QuadrantBackIndex = std::min(session.QuadrantBackIndex, paintQuadrantIndex);
    session.QuadrantFrontIndex = std::max(session.QuadrantFrontIndex, paintQuadrantIndex);
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
    paint_session& session, ImageId image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize,
    const CoordsXYZ& boundBoxOffset)
{
    auto* const g1 = gfx_get_g1_element(image_id);
    if (g1 == nullptr)
    {
        return nullptr;
    }

    const auto swappedRotation = DirectionFlipXAxis(session.CurrentRotation);
    auto swappedRotCoord = CoordsXYZ{ offset.Rotate(swappedRotation), offset.z };
    swappedRotCoord += session.SpritePosition;

    const auto imagePos = translate_3d_to_2d_with_z(session.CurrentRotation, swappedRotCoord);

    if (!ImageWithinDPI(imagePos, *g1, session.DPI))
    {
        return nullptr;
    }

    const auto rotBoundBoxOffset = CoordsXYZ{ boundBoxOffset.Rotate(swappedRotation), boundBoxOffset.z };
    const auto rotBoundBoxSize = RotateBoundBoxSize(boundBoxSize, session.CurrentRotation);

    auto* ps = session.AllocateNormalPaintEntry();
    if (ps == nullptr)
    {
        return nullptr;
    }

    ps->image_id = image_id;
    ps->x = imagePos.x;
    ps->y = imagePos.y;
    ps->bounds.x_end = rotBoundBoxSize.x + rotBoundBoxOffset.x + session.SpritePosition.x;
    ps->bounds.y_end = rotBoundBoxSize.y + rotBoundBoxOffset.y + session.SpritePosition.y;
    ps->bounds.z_end = rotBoundBoxSize.z + rotBoundBoxOffset.z;
    ps->bounds.x = rotBoundBoxOffset.x + session.SpritePosition.x;
    ps->bounds.y = rotBoundBoxOffset.y + session.SpritePosition.y;
    ps->bounds.z = rotBoundBoxOffset.z;
    ps->attached_ps = nullptr;
    ps->children = nullptr;
    ps->sprite_type = session.InteractionType;
    ps->map_x = session.MapPosition.x;
    ps->map_y = session.MapPosition.y;
    ps->tileElement = session.CurrentlyDrawnTileElement;
    ps->entity = session.CurrentlyDrawnEntity;

    return ps;
}

template<uint8_t direction> void PaintSessionGenerateRotate(paint_session& session)
{
    // Optimised modified version of viewport_coord_to_map_coord
    ScreenCoordsXY screenCoord = { floor2(session.DPI.x, 32), floor2((session.DPI.y - 16), 32) };
    CoordsXY mapTile = { screenCoord.y - screenCoord.x / 2, screenCoord.y + screenCoord.x / 2 };
    mapTile = mapTile.Rotate(direction);

    if constexpr (direction & 1)
    {
        mapTile.y -= 16;
    }
    mapTile = mapTile.ToTileStart();

    uint16_t numVerticalTiles = (session.DPI.height + 2128) >> 5;

    // Adjacent tiles to also check due to overlapping of sprites
    constexpr CoordsXY adjacentTiles[] = {
        CoordsXY{ -32, 32 }.Rotate(direction),
        CoordsXY{ 0, 32 }.Rotate(direction),
        CoordsXY{ 32, 0 }.Rotate(direction),
    };
    constexpr CoordsXY nextVerticalTile = CoordsXY{ 32, 32 }.Rotate(direction);

    for (; numVerticalTiles > 0; --numVerticalTiles)
    {
        tile_element_paint_setup(session, mapTile);
        EntityPaintSetup(session, mapTile);

        const auto loc1 = mapTile + adjacentTiles[0];
        EntityPaintSetup(session, loc1);

        const auto loc2 = mapTile + adjacentTiles[1];
        tile_element_paint_setup(session, loc2);
        EntityPaintSetup(session, loc2);

        const auto loc3 = mapTile + adjacentTiles[2];
        EntityPaintSetup(session, loc3);

        mapTile += nextVerticalTile;
    }
}

/**
 *
 *  rct2: 0x0068B6C2
 */
void PaintSessionGenerate(paint_session& session)
{
    session.CurrentRotation = get_current_rotation();
    switch (DirectionFlipXAxis(session.CurrentRotation))
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

namespace PaintSortFlags
{
    static constexpr uint8_t None = 0;
    static constexpr uint8_t PendingVisit = (1U << 0);
    static constexpr uint8_t Neighbour = (1U << 1);
    static constexpr uint8_t OutsideQuadrant = (1U << 7);
} // namespace PaintSortFlags

template<uint8_t TRotation>
static paint_struct* PaintArrangeStructsHelperRotation(paint_struct* ps_next, uint16_t quadrantIndex, uint8_t flag)
{
    paint_struct* ps;
    paint_struct* ps_temp;

    // Get the first node in the specified quadrant.
    do
    {
        ps = ps_next;
        ps_next = ps_next->next_quadrant_ps;
        if (ps_next == nullptr)
            return ps;
    } while (quadrantIndex > ps_next->quadrant_index);

    // We keep track of the first node in the quadrant so the next call with a higher quadrant index
    // can use this node to skip some iterations.
    paint_struct* psQuadrantEntry = ps;

    // Visit all nodes in the linked quadrant list and determine their current
    // sorting relevancy.
    ps_temp = ps;
    do
    {
        ps = ps->next_quadrant_ps;
        if (ps == nullptr)
            break;

        if (ps->quadrant_index > quadrantIndex + 1)
        {
            // Outside of the range.
            ps->SortFlags = PaintSortFlags::OutsideQuadrant;
        }
        else if (ps->quadrant_index == quadrantIndex + 1)
        {
            // Is neighbour and requires a visit.
            ps->SortFlags = PaintSortFlags::Neighbour | PaintSortFlags::PendingVisit;
        }
        else if (ps->quadrant_index == quadrantIndex)
        {
            // In specified quadrant, requires visit.
            ps->SortFlags = flag | PaintSortFlags::PendingVisit;
        }
    } while (ps->quadrant_index <= quadrantIndex + 1);
    ps = ps_temp;

    // Iterate all nodes in the current list and re-order them based on
    // the current rotation and their bounding box.
    while (true)
    {
        // Get the first pending node in the quadrant list
        while (true)
        {
            ps_next = ps->next_quadrant_ps;
            if (ps_next == nullptr)
            {
                // End of the current list.
                return psQuadrantEntry;
            }
            if (ps_next->SortFlags & PaintSortFlags::OutsideQuadrant)
            {
                // Reached point outside of specified quadrant.
                return psQuadrantEntry;
            }
            if (ps_next->SortFlags & PaintSortFlags::PendingVisit)
            {
                // Found node to check on.
                break;
            }
            ps = ps_next;
        }

        // Mark visited.
        ps_next->SortFlags &= ~PaintSortFlags::PendingVisit;
        ps_temp = ps;

        // Compare current node against the remaining children.
        const paint_struct_bound_box& initialBBox = ps_next->bounds;
        while (true)
        {
            ps = ps_next;
            ps_next = ps_next->next_quadrant_ps;
            if (ps_next == nullptr)
                break;
            if (ps_next->SortFlags & PaintSortFlags::OutsideQuadrant)
                break;
            if (!(ps_next->SortFlags & PaintSortFlags::Neighbour))
                continue;

            const paint_struct_bound_box& currentBBox = ps_next->bounds;

            const bool compareResult = CheckBoundingBox<TRotation>(initialBBox, currentBBox);

            if (compareResult)
            {
                // Child node intersects with current node, move behind.
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

template<int TRotation> static void PaintSessionArrange(PaintSessionCore& session, bool)
{
    paint_struct* psHead = &session.PaintHead;

    paint_struct* ps = psHead;
    ps->next_quadrant_ps = nullptr;

    uint32_t quadrantIndex = session.QuadrantBackIndex;
    if (quadrantIndex != UINT32_MAX)
    {
        do
        {
            paint_struct* ps_next = session.Quadrants[quadrantIndex];
            if (ps_next != nullptr)
            {
                ps->next_quadrant_ps = ps_next;
                do
                {
                    ps = ps_next;
                    ps_next = ps_next->next_quadrant_ps;

                } while (ps_next != nullptr);
            }
        } while (++quadrantIndex <= session.QuadrantFrontIndex);

        paint_struct* ps_cache = PaintArrangeStructsHelperRotation<TRotation>(
            psHead, session.QuadrantBackIndex & 0xFFFF, PaintSortFlags::Neighbour);

        quadrantIndex = session.QuadrantBackIndex;
        while (++quadrantIndex < session.QuadrantFrontIndex)
        {
            ps_cache = PaintArrangeStructsHelperRotation<TRotation>(ps_cache, quadrantIndex & 0xFFFF, PaintSortFlags::None);
        }
    }
}

/**
 *
 *  rct2: 0x00688217
 */
void PaintSessionArrange(PaintSessionCore& session)
{
    PROFILED_FUNCTION();
    switch (session.CurrentRotation)
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

static void PaintDrawStruct(paint_session& session, paint_struct* ps)
{
    rct_drawpixelinfo* dpi = &session.DPI;

    auto x = ps->x;
    auto y = ps->y;

    if (ps->sprite_type == ViewportInteractionItem::Entity)
    {
        if (dpi->zoom_level >= ZoomLevel{ 1 })
        {
            x = floor2(x, 2);
            y = floor2(y, 2);
            if (dpi->zoom_level >= ZoomLevel{ 2 })
            {
                x = floor2(x, 4);
                y = floor2(y, 4);
            }
        }
    }

    auto imageId = PaintPSColourifyImage(ps, ps->image_id, session.ViewFlags);
    if (gPaintBoundingBoxes && dpi->zoom_level == ZoomLevel{ 0 })
    {
        PaintPSImageWithBoundingBoxes(dpi, ps, imageId, x, y);
    }
    else
    {
        gfx_draw_sprite(dpi, imageId, { x, y });
    }

    if (ps->children != nullptr)
    {
        PaintDrawStruct(session, ps->children);
    }
    else
    {
        PaintAttachedPS(dpi, ps, session.ViewFlags);
    }
}

/**
 *
 *  rct2: 0x00688485
 */
void PaintDrawStructs(paint_session& session)
{
    PROFILED_FUNCTION();

    paint_struct* ps = &session.PaintHead;

    for (ps = ps->next_quadrant_ps; ps != nullptr;)
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
    for (; attached_ps != nullptr; attached_ps = attached_ps->next)
    {
        auto screenCoords = ScreenCoordsXY{ attached_ps->x + ps->x, attached_ps->y + ps->y };

        auto imageId = PaintPSColourifyImage(ps, attached_ps->image_id, viewFlags);
        if (attached_ps->IsMasked)
        {
            gfx_draw_sprite_raw_masked(dpi, screenCoords, imageId, attached_ps->ColourImageId);
        }
        else
        {
            gfx_draw_sprite(dpi, imageId, screenCoords);
        }
    }
}

static void PaintPSImageWithBoundingBoxes(rct_drawpixelinfo* dpi, paint_struct* ps, ImageId imageId, int32_t x, int32_t y)
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

    gfx_draw_sprite(dpi, imageId, { x, y });

    // vertical front
    gfx_draw_line(dpi, { screenCoordFrontTop, screenCoordFrontBottom }, colour);

    // top square
    gfx_draw_line(dpi, { screenCoordFrontTop, screenCoordLeftTop }, colour);
    gfx_draw_line(dpi, { screenCoordFrontTop, screenCoordRightTop }, colour);
}

static ImageId PaintPSColourifyImage(const paint_struct* ps, ImageId imageId, uint32_t viewFlags)
{
    auto visibility = GetPaintStructVisibility(ps, viewFlags);
    switch (visibility)
    {
        case VisibilityKind::Partial:
            return imageId.WithTransparancy(FilterPaletteID::PaletteDarken1);
        case VisibilityKind::Hidden:
            return ImageId();
        default:
            return imageId;
    }
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
    paint_session& session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize)
{
    return PaintAddImageAsParent(session, ImageId::FromUInt32(image_id), offset, boundBoxSize, offset);
}

paint_struct* PaintAddImageAsParent(
    paint_session& session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize,
    const CoordsXYZ& boundBoxOffset)
{
    return PaintAddImageAsParent(session, ImageId::FromUInt32(image_id), offset, boundBoxSize, boundBoxOffset);
}

paint_struct* PaintAddImageAsParent(
    paint_session& session, ImageId imageId, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize)
{
    return PaintAddImageAsParent(session, imageId, offset, boundBoxSize, offset);
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
    paint_session& session, ImageId image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize,
    const CoordsXYZ& boundBoxOffset)
{
    session.LastPS = nullptr;
    session.LastAttachedPS = nullptr;

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
    paint_session& session, ImageId imageId, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize,
    const CoordsXYZ& boundBoxOffset)
{
    session.LastPS = nullptr;
    session.LastAttachedPS = nullptr;
    return CreateNormalPaintStruct(session, imageId, offset, boundBoxSize, boundBoxOffset);
}

paint_struct* PaintAddImageAsChild(
    paint_session& session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxLength,
    const CoordsXYZ& boundBoxOffset)
{
    return PaintAddImageAsChild(session, ImageId::FromUInt32(image_id), offset, boundBoxLength, boundBoxOffset);
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
    paint_session& session, ImageId image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxLength,
    const CoordsXYZ& boundBoxOffset)
{
    paint_struct* parentPS = session.LastPS;
    if (parentPS == nullptr)
    {
        return PaintAddImageAsParent(session, image_id, offset, boundBoxLength, boundBoxOffset);
    }

    auto* ps = CreateNormalPaintStruct(session, image_id, offset, boundBoxLength, boundBoxOffset);
    if (ps == nullptr)
    {
        return nullptr;
    }

    parentPS->children = ps;

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
bool PaintAttachToPreviousAttach(paint_session& session, ImageId imageId, int32_t x, int32_t y)
{
    auto* previousAttachedPS = session.LastAttachedPS;
    if (previousAttachedPS == nullptr)
    {
        return PaintAttachToPreviousPS(session, imageId, x, y);
    }

    auto* ps = session.AllocateAttachedPaintEntry();
    if (ps == nullptr)
    {
        return false;
    }

    ps->image_id = imageId;
    ps->x = x;
    ps->y = y;
    ps->IsMasked = false;
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
bool PaintAttachToPreviousPS(paint_session& session, uint32_t image_id, int32_t x, int32_t y)
{
    return PaintAttachToPreviousPS(session, ImageId::FromUInt32(image_id), x, y);
}

bool PaintAttachToPreviousPS(paint_session& session, ImageId image_id, int32_t x, int32_t y)
{
    auto* masterPs = session.LastPS;
    if (masterPs == nullptr)
    {
        return false;
    }

    auto* ps = session.AllocateAttachedPaintEntry();
    if (ps == nullptr)
    {
        return false;
    }

    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->IsMasked = false;

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
    paint_session& session, money64 amount, rct_string_id string_id, int32_t y, int32_t z, int8_t y_offsets[], int32_t offset_x,
    uint32_t rotation)
{
    auto* ps = session.AllocateStringPaintEntry();
    if (ps == nullptr)
    {
        return;
    }

    const CoordsXYZ position = {
        session.SpritePosition.x,
        session.SpritePosition.y,
        z,
    };
    const auto coord = translate_3d_to_2d_with_z(rotation, position);

    ps->string_id = string_id;
    ps->next = nullptr;
    std::memcpy(ps->args, &amount, sizeof(amount));
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
    auto context = OpenRCT2::GetContext();
    do
    {
        char buffer[256]{};
        format_string(buffer, sizeof(buffer), ps->string_id, &ps->args);

        // Use sprite font unless the currency contains characters unsupported by the sprite font
        auto forceSpriteFont = false;
        const auto& currencyDesc = CurrencyDescriptors[EnumValue(gConfigGeneral.currency_format)];
        if (LocalisationService_UseTrueTypeFont(context) && font_supports_string_sprite(currencyDesc.symbol_unicode))
        {
            forceSpriteFont = true;
        }

        gfx_draw_string_with_y_offsets(
            context, dpi, buffer, COLOUR_BLACK, { ps->x, ps->y }, reinterpret_cast<int8_t*>(ps->y_offsets), forceSpriteFont,
            FontSpriteBase::MEDIUM);
    } while ((ps = ps->next) != nullptr);
}

PaintEntryPool::Chain::Chain(PaintEntryPool* pool)
    : Pool(pool)
{
}

PaintEntryPool::Chain::Chain(Chain&& chain)
{
    *this = std::move(chain);
}

PaintEntryPool::Chain::~Chain()
{
    Clear();
}

PaintEntryPool::Chain& PaintEntryPool::Chain::operator=(Chain&& chain) noexcept
{
    Pool = chain.Pool;
    Head = chain.Head;
    Current = chain.Current;
    chain.Pool = nullptr;
    chain.Head = nullptr;
    chain.Current = nullptr;
    return *this;
}

paint_entry* PaintEntryPool::Chain::Allocate()
{
    if (Pool == nullptr)
    {
        return nullptr;
    }

    if (Current == nullptr)
    {
        assert(Head == nullptr);
        Head = Pool->AllocateNode();
        if (Head == nullptr)
        {
            // Unable to allocate any more nodes
            return nullptr;
        }
        Current = Head;
    }
    else if (Current->Count >= NodeSize)
    {
        // We need another node
        Current->Next = Pool->AllocateNode();
        if (Current->Next == nullptr)
        {
            // Unable to allocate any more nodes
            return nullptr;
        }
        Current = Current->Next;
    }

    assert(Current->Count < NodeSize);
    return &Current->PaintStructs[Current->Count++];
}

void PaintEntryPool::Chain::Clear()
{
    if (Pool != nullptr)
    {
        Pool->FreeNodes(Head);
        Head = nullptr;
        Current = nullptr;
    }
    assert(Head == nullptr);
    assert(Current == nullptr);
}

size_t PaintEntryPool::Chain::GetCount() const
{
    size_t count = 0;
    auto current = Head;
    while (current != nullptr)
    {
        count += current->Count;
        current = current->Next;
    }
    return count;
}

PaintEntryPool::~PaintEntryPool()
{
    for (auto node : _available)
    {
        delete node;
    }
    _available.clear();
}

PaintEntryPool::Node* PaintEntryPool::AllocateNode()
{
    std::lock_guard<std::mutex> lock(_mutex);

    PaintEntryPool::Node* result;
    if (_available.size() > 0)
    {
        result = _available.back();
        _available.pop_back();
    }
    else
    {
        result = new (std::nothrow) PaintEntryPool::Node();
    }
    return result;
}

PaintEntryPool::Chain PaintEntryPool::Create()
{
    return PaintEntryPool::Chain(this);
}

void PaintEntryPool::FreeNodes(PaintEntryPool::Node* head)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto node = head;
    while (node != nullptr)
    {
        auto next = node->Next;
        node->Next = nullptr;
        node->Count = 0;
        _available.push_back(node);
        node = next;
    }
}
