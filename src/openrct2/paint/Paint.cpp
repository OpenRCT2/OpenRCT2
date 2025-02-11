/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../core/Money.hpp"
#include "../core/Numerics.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../localisation/Currency.h"
#include "../localisation/Formatting.h"
#include "../localisation/LocalisationService.h"
#include "../paint/Painter.h"
#include "../platform/Memory.h"
#include "../profiling/Profiling.h"
#include "Boundbox.h"
#include "Paint.Entity.h"
#include "tile_element/Paint.TileElement.h"

#include <algorithm>
#include <array>
#include <cassert>

using namespace OpenRCT2;
using namespace OpenRCT2::Numerics;

// Globals for paint clipping
uint8_t gClipHeight = 128; // Default to middle value
CoordsXY gClipSelectionA = { 0, 0 };
CoordsXY gClipSelectionB = { kMaximumTileStartXY, kMaximumTileStartXY };

static constexpr uint8_t BoundBoxDebugColours[] = {
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
bool gPaintStableSort;

static void PaintPSImageWithBoundingBoxes(PaintSession& session, PaintStruct* ps, ImageId imageId, int32_t x, int32_t y);
static ImageId PaintPSColourifyImage(const PaintStruct* ps, ImageId imageId, uint32_t viewFlags);

static int32_t RemapPositionToQuadrant(const PaintStruct& ps, uint8_t rotation)
{
    constexpr auto MapRangeMax = MaxPaintQuadrants * kCoordsXYStep;
    constexpr auto MapRangeCenter = MapRangeMax / 2;

    const auto x = ps.Bounds.x;
    const auto y = ps.Bounds.y;
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

static void PaintSessionAddPSToQuadrant(PaintSession& session, PaintStruct* ps)
{
    const auto positionHash = RemapPositionToQuadrant(*ps, session.CurrentRotation);

    // Values below zero or above MaxPaintQuadrants are void, corners also share the same quadrant as void.
    const uint32_t paintQuadrantIndex = std::clamp(positionHash / kCoordsXYStep, 0, MaxPaintQuadrants - 1);

    ps->QuadrantIndex = paintQuadrantIndex;
    ps->NextQuadrantEntry = session.Quadrants[paintQuadrantIndex];
    session.Quadrants[paintQuadrantIndex] = ps;

    session.QuadrantBackIndex = std::min(session.QuadrantBackIndex, paintQuadrantIndex);
    session.QuadrantFrontIndex = std::max(session.QuadrantFrontIndex, paintQuadrantIndex);
}

static constexpr bool ImageWithinDPI(const ScreenCoordsXY& imagePos, const G1Element& g1, const DrawPixelInfo& dpi)
{
    int32_t left = imagePos.x + g1.x_offset;
    int32_t bottom = imagePos.y + g1.y_offset;

    int32_t right = left + g1.width;
    int32_t top = bottom + g1.height;

    // mber: It is possible to use only the bottom else block here if you change <= and >= to simply < and >.
    // However, since this is used to cull paint structs, I'd prefer to keep the condition strict and calculate
    // the culling differently for minifying and magnifying.
    auto zoom = dpi.zoom_level;
    if (zoom > ZoomLevel{ 0 })
    {
        if (right <= dpi.WorldX())
            return false;
        if (top <= dpi.WorldY())
            return false;
        if (left >= dpi.WorldX() + dpi.WorldWidth())
            return false;
        if (bottom >= dpi.WorldY() + dpi.WorldHeight())
            return false;
    }
    else
    {
        if (zoom.ApplyInversedTo(right) <= dpi.x)
            return false;
        if (zoom.ApplyInversedTo(top) <= dpi.y)
            return false;
        if (zoom.ApplyInversedTo(left) >= dpi.x + dpi.width)
            return false;
        if (zoom.ApplyInversedTo(bottom) >= dpi.y + dpi.height)
            return false;
    }
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
static PaintStruct* CreateNormalPaintStruct(
    PaintSession& session, ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    auto* const g1 = GfxGetG1Element(image_id);
    if (g1 == nullptr)
    {
        return nullptr;
    }

    const auto swappedRotation = DirectionFlipXAxis(session.CurrentRotation);
    auto swappedRotCoord = CoordsXYZ{ offset.Rotate(swappedRotation), offset.z };
    swappedRotCoord += session.SpritePosition;

    const auto imagePos = Translate3DTo2DWithZ(session.CurrentRotation, swappedRotCoord);

    if (!ImageWithinDPI(imagePos, *g1, session.DPI))
    {
        return nullptr;
    }

    const auto rotBoundBoxOffset = CoordsXYZ{ boundBox.offset.Rotate(swappedRotation), boundBox.offset.z };
    const auto rotBoundBoxSize = RotateBoundBoxSize(boundBox.length, session.CurrentRotation);

    auto* ps = session.AllocateNormalPaintEntry();
    if (ps == nullptr)
    {
        return nullptr;
    }

    ps->image_id = image_id;
    ps->ScreenPos = imagePos;
    ps->Bounds.x_end = rotBoundBoxSize.x + rotBoundBoxOffset.x + session.SpritePosition.x;
    ps->Bounds.y_end = rotBoundBoxSize.y + rotBoundBoxOffset.y + session.SpritePosition.y;
    ps->Bounds.z_end = rotBoundBoxSize.z + rotBoundBoxOffset.z;
    ps->Bounds.x = rotBoundBoxOffset.x + session.SpritePosition.x;
    ps->Bounds.y = rotBoundBoxOffset.y + session.SpritePosition.y;
    ps->Bounds.z = rotBoundBoxOffset.z;
    ps->Attached = nullptr;
    ps->Children = nullptr;
    ps->NextQuadrantEntry = nullptr;
    ps->InteractionItem = session.InteractionType;
    ps->MapPos = session.MapPosition;
    ps->Element = session.CurrentlyDrawnTileElement;
    ps->Entity = session.CurrentlyDrawnEntity;

    return ps;
}

template<uint8_t direction>
void PaintSessionGenerateRotate(PaintSession& session)
{
    // Optimised modified version of ViewportPosToMapPos
    ScreenCoordsXY screenCoord = { floor2(session.DPI.WorldX(), 32), floor2((session.DPI.WorldY() - 16), 32) };
    CoordsXY mapTile = { screenCoord.y - screenCoord.x / 2, screenCoord.y + screenCoord.x / 2 };
    mapTile = mapTile.Rotate(direction);

    if constexpr (direction & 1)
    {
        mapTile.y -= 16;
    }
    mapTile = mapTile.ToTileStart();

    uint16_t numVerticalTiles = (session.DPI.WorldHeight() + 2128) >> 5;

    // Adjacent tiles to also check due to overlapping of sprites
    constexpr CoordsXY adjacentTiles[] = {
        CoordsXY{ -32, 32 }.Rotate(direction),
        CoordsXY{ 0, 32 }.Rotate(direction),
        CoordsXY{ 32, 0 }.Rotate(direction),
    };
    constexpr CoordsXY nextVerticalTile = CoordsXY{ 32, 32 }.Rotate(direction);

    for (; numVerticalTiles > 0; --numVerticalTiles)
    {
        TileElementPaintSetup(session, mapTile);
        EntityPaintSetup(session, mapTile);

        const auto loc1 = mapTile + adjacentTiles[0];
        EntityPaintSetup(session, loc1);

        const auto loc2 = mapTile + adjacentTiles[1];
        TileElementPaintSetup(session, loc2);
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
void PaintSessionGenerate(PaintSession& session)
{
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

template<uint8_t TRotation>
static bool CheckBoundingBox(const PaintStructBoundBox& initialBBox, const PaintStructBoundBox& currentBBox)
{
    if constexpr (TRotation == 0)
    {
        if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end >= currentBBox.y && initialBBox.x_end >= currentBBox.x
            && !(initialBBox.z < currentBBox.z_end && initialBBox.y < currentBBox.y_end && initialBBox.x < currentBBox.x_end))
        {
            return true;
        }
    }
    else if constexpr (TRotation == 1)
    {
        if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end >= currentBBox.y && initialBBox.x_end < currentBBox.x
            && !(initialBBox.z < currentBBox.z_end && initialBBox.y < currentBBox.y_end && initialBBox.x >= currentBBox.x_end))
        {
            return true;
        }
    }
    else if constexpr (TRotation == 2)
    {
        if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end < currentBBox.y && initialBBox.x_end < currentBBox.x
            && !(initialBBox.z < currentBBox.z_end && initialBBox.y >= currentBBox.y_end && initialBBox.x >= currentBBox.x_end))
        {
            return true;
        }
    }
    else if constexpr (TRotation == 3)
    {
        if (initialBBox.z_end >= currentBBox.z && initialBBox.y_end < currentBBox.y && initialBBox.x_end >= currentBBox.x
            && !(initialBBox.z < currentBBox.z_end && initialBBox.y >= currentBBox.y_end && initialBBox.x < currentBBox.x_end))
        {
            return true;
        }
    }
    return false;
}

namespace OpenRCT2::PaintSortFlags
{
    static constexpr uint8_t None = 0;
    static constexpr uint8_t PendingVisit = (1u << 0);
    static constexpr uint8_t Neighbour = (1u << 1);
    static constexpr uint8_t OutsideQuadrant = (1u << 7);
} // namespace OpenRCT2::PaintSortFlags

static PaintStruct* PaintStructsFirstInQuadrant(PaintStruct* psNext, uint16_t quadrantIndex)
{
    PaintStruct* ps;
    do
    {
        ps = psNext;
        psNext = psNext->NextQuadrantEntry;
        if (psNext == nullptr)
            return ps;
    } while (quadrantIndex > psNext->QuadrantIndex);
    return ps;
}

// Initializes sorting flags for all entries in the specified quadrant by quadrantIndex.
// Sorting flags specify whether a node needs to be traversed, is a neighbour, or is outside the
// quadrant range.
static void PaintStructsInitializeSort(PaintStruct* ps, uint16_t quadrantIndex, uint8_t flag)
{
    do
    {
        ps = ps->NextQuadrantEntry;
        if (ps == nullptr)
            break;

        if (ps->QuadrantIndex > quadrantIndex + 1)
        {
            // Outside of the range.
            ps->SortFlags = PaintSortFlags::OutsideQuadrant;
        }
        else if (ps->QuadrantIndex == quadrantIndex + 1)
        {
            // Is neighbour and requires a visit.
            ps->SortFlags = PaintSortFlags::Neighbour | PaintSortFlags::PendingVisit;
        }
        else if (ps->QuadrantIndex == quadrantIndex)
        {
            // In specified quadrant, requires visit.
            ps->SortFlags = flag | PaintSortFlags::PendingVisit;
        }
    } while (ps->QuadrantIndex <= quadrantIndex + 1);
}

// Returns a pair of parent and child where child is the next node that requires traversal.
// Because this structure uses a singly linked list we need to keep track of the parent in order
// to be able to re-order the list.
static std::pair<PaintStruct*, PaintStruct*> PaintStructsGetNextPending(PaintStruct* ps)
{
    PaintStruct* ps_next;
    while (true)
    {
        ps_next = ps->NextQuadrantEntry;
        if (ps_next == nullptr)
        {
            // End of the current list.
            return { nullptr, nullptr };
        }
        if (ps_next->SortFlags & PaintSortFlags::OutsideQuadrant)
        {
            // Reached point outside of specified quadrant.
            return { nullptr, nullptr };
        }
        if (ps_next->SortFlags & PaintSortFlags::PendingVisit)
        {
            // Found node to check on.
            break;
        }
        ps = ps_next;
    }
    return { ps, ps_next };
}

// Re-orders all nodes after the specified child node and marks the child node as traversed. The resulting
// order of the children is the depth based on rotation and dimensions of the bounding box.
template<uint8_t TRotation>
static void PaintStructsSortQuadrantLegacy(PaintStruct* parent, PaintStruct* child)
{
    // Mark visited.
    child->SortFlags &= ~PaintSortFlags::PendingVisit;

    // Compare all the children below the first child and move them up in the list if they intersect.
    const PaintStructBoundBox& initialBBox = child->Bounds;

    for (;;)
    {
        auto* ps = child;
        child = child->NextQuadrantEntry;

        if (child != nullptr)
        {
            PREFETCH(&child->Bounds);
        }
        if (child == nullptr || child->SortFlags & PaintSortFlags::OutsideQuadrant)
        {
            break;
        }

        if (!(child->SortFlags & PaintSortFlags::Neighbour))
        {
            continue;
        }

        if (CheckBoundingBox<TRotation>(initialBBox, child->Bounds))
        {
            // Child node intersects with current node, move behind.
            ps->NextQuadrantEntry = child->NextQuadrantEntry;

            auto* psTemp = parent->NextQuadrantEntry;
            parent->NextQuadrantEntry = child;

            child->NextQuadrantEntry = psTemp;
            child = ps;
        }
    }
}

// Re-orders all nodes after the specified child node and marks the child node as traversed. The resulting
// order of the children is the depth based on rotation and dimensions of the bounding box.
template<uint8_t TRotation>
static void PaintStructsSortQuadrantStable(PaintStruct* parent, PaintStruct* child)
{
    // Mark visited.
    child->SortFlags &= ~PaintSortFlags::PendingVisit;

    // Compare all the children below the first child and move them up in the list if they intersect.
    const PaintStructBoundBox& initialBBox = child->Bounds;

    // Create a temporary list to collect sorted nodes in stable order.
    PaintStruct* sortedHead = nullptr;
    PaintStruct* sortedTail = nullptr;

    // Traverse the list and reorder based on intersection.
    for (;;)
    {
        PaintStruct* next = child->NextQuadrantEntry;

        if (next != nullptr)
        {
            PREFETCH(&next->Bounds);
        }

        // Stop if at the end of the list or outside the quadrant range.
        if (next == nullptr || next->SortFlags & PaintSortFlags::OutsideQuadrant)
        {
            break;
        }

        // Ignore nodes that are not neighbors.
        if (!(next->SortFlags & PaintSortFlags::Neighbour))
        {
            child = next;
            continue;
        }

        // Detach the current node from the list if it intersects.
        if (CheckBoundingBox<TRotation>(initialBBox, next->Bounds))
        {
            child->NextQuadrantEntry = next->NextQuadrantEntry;

            if (sortedHead == nullptr)
            {
                sortedHead = next;
                sortedTail = next;
                next->NextQuadrantEntry = nullptr;
            }
            else
            {
                sortedTail->NextQuadrantEntry = next;
                sortedTail = next;
                next->NextQuadrantEntry = nullptr;
            }
        }
        else
        {
            child = next;
        }
    }

    // Merge the sorted list back into the main list after parent.
    if (sortedHead != nullptr)
    {
        PaintStruct* originalNext = parent->NextQuadrantEntry;
        parent->NextQuadrantEntry = sortedHead;
        sortedTail->NextQuadrantEntry = originalNext;
    }
}

template<bool TStableSort, uint8_t TRotation>
static PaintStruct* PaintArrangeStructsHelperRotation(PaintStruct* psQuadrantEntry, uint16_t quadrantIndex, uint8_t flag)
{
    // We keep track of the first node in the quadrant so the next call with a higher quadrant index
    // can use this node to skip some iterations.
    psQuadrantEntry = PaintStructsFirstInQuadrant(psQuadrantEntry, quadrantIndex);

    // Visit all nodes in the linked quadrant list and determine their current
    // sorting relevancy.
    PaintStructsInitializeSort(psQuadrantEntry, quadrantIndex, flag);

    // Iterate all nodes in the current list and re-order them based on
    // the current rotation and their bounding box.
    for (auto* ps = psQuadrantEntry; ps != nullptr;)
    {
        const auto [parent, child] = PaintStructsGetNextPending(ps);
        if (parent == nullptr)
        {
            break;
        }

        if constexpr (TStableSort)
        {
            PaintStructsSortQuadrantStable<TRotation>(parent, child);
        }
        else
        {
            PaintStructsSortQuadrantLegacy<TRotation>(parent, child);
        }

        ps = parent;
    }

    return psQuadrantEntry;
}

// Iterates over all the quadrant lists and links them together as a
// singly linked list.
// The paint session has a head member which is the first entry.
static void PaintStructsLinkQuadrants(PaintSessionCore& session, PaintStruct& psHead)
{
    PaintStruct* ps = &psHead;
    ps->NextQuadrantEntry = nullptr;

    uint32_t quadrantIndex = session.QuadrantBackIndex;
    do
    {
        PaintStruct* psNext = session.Quadrants[quadrantIndex];
        if (psNext != nullptr)
        {
            ps->NextQuadrantEntry = psNext;
            do
            {
                ps = psNext;
                psNext = psNext->NextQuadrantEntry;

            } while (psNext != nullptr);
        }
    } while (++quadrantIndex <= session.QuadrantFrontIndex);
}

template<bool TStableSort, int TRotation>
static void PaintSessionArrangeImpl(PaintSessionCore& session)
{
    uint32_t quadrantIndex = session.QuadrantBackIndex;
    if (quadrantIndex == UINT32_MAX)
    {
        return;
    }

    // psHead is an intermediate node that is used to link all the quadrant lists together,
    // this was previously stored in PaintSession but only the NextQuadrantEntry is relevant here.
    // The head node is not part of the linked list and just serves as an entry point.
    PaintStruct psHead{};
    PaintStructsLinkQuadrants(session, psHead);

    PaintStruct* psNextQuadrant = PaintArrangeStructsHelperRotation<TStableSort, TRotation>(
        &psHead, session.QuadrantBackIndex, PaintSortFlags::Neighbour);

    while (++quadrantIndex < session.QuadrantFrontIndex)
    {
        psNextQuadrant = PaintArrangeStructsHelperRotation<TStableSort, TRotation>(
            psNextQuadrant, quadrantIndex, PaintSortFlags::None);
    }

    session.PaintHead = psHead.NextQuadrantEntry;
}

using PaintArrangeWithRotation = void (*)(PaintSessionCore& session);

constexpr std::array _paintArrangeFuncsLegacy = {
    PaintSessionArrangeImpl<false, 0>,
    PaintSessionArrangeImpl<false, 1>,
    PaintSessionArrangeImpl<false, 2>,
    PaintSessionArrangeImpl<false, 3>,
};

constexpr std::array _paintArrangeFuncsStable = {
    PaintSessionArrangeImpl<true, 0>,
    PaintSessionArrangeImpl<true, 1>,
    PaintSessionArrangeImpl<true, 2>,
    PaintSessionArrangeImpl<true, 3>,
};

/**
 *
 *  rct2: 0x00688217
 */
void PaintSessionArrange(PaintSessionCore& session)
{
    PROFILED_FUNCTION();
    if (gPaintStableSort)
    {
        return _paintArrangeFuncsStable[session.CurrentRotation](session);
    }
    return _paintArrangeFuncsLegacy[session.CurrentRotation](session);
}

static inline void PaintAttachedPS(DrawPixelInfo& dpi, PaintStruct* ps, uint32_t viewFlags)
{
    AttachedPaintStruct* attached_ps = ps->Attached;
    for (; attached_ps != nullptr; attached_ps = attached_ps->NextEntry)
    {
        const auto screenCoords = ps->ScreenPos + attached_ps->RelativePos;

        auto imageId = PaintPSColourifyImage(ps, attached_ps->image_id, viewFlags);
        if (attached_ps->IsMasked)
        {
            GfxDrawSpriteRawMasked(dpi, screenCoords, imageId, attached_ps->ColourImageId);
        }
        else
        {
            GfxDrawSprite(dpi, imageId, screenCoords);
        }
    }
}

static inline void PaintDrawStruct(PaintSession& session, PaintStruct* ps)
{
    auto screenPos = ps->ScreenPos;
    if (ps->InteractionItem == ViewportInteractionItem::Entity)
    {
        if (session.DPI.zoom_level >= ZoomLevel{ 1 })
        {
            screenPos.x = floor2(screenPos.x, 2);
            screenPos.y = floor2(screenPos.y, 2);
            if (session.DPI.zoom_level >= ZoomLevel{ 2 })
            {
                screenPos.x = floor2(screenPos.x, 4);
                screenPos.y = floor2(screenPos.y, 4);
            }
        }
    }
    auto imageId = PaintPSColourifyImage(ps, ps->image_id, session.ViewFlags);
    if (gPaintBoundingBoxes)
    {
        PaintPSImageWithBoundingBoxes(session, ps, imageId, screenPos.x, screenPos.y);
    }
    else
    {
        GfxDrawSprite(session.DPI, imageId, screenPos);
    }

    if (ps->Children != nullptr)
    {
        PaintDrawStruct(session, ps->Children);
    }
    else
    {
        PaintAttachedPS(session.DPI, ps, session.ViewFlags);
    }
}

/**
 *
 *  rct2: 0x00688485
 */
void PaintDrawStructs(PaintSession& session)
{
    PROFILED_FUNCTION();

    for (PaintStruct* ps = session.PaintHead; ps != nullptr; ps = ps->NextQuadrantEntry)
    {
        PaintDrawStruct(session, ps);
    }
}

static void PaintPSImageWithBoundingBoxes(PaintSession& session, PaintStruct* ps, ImageId imageId, int32_t x, int32_t y)
{
    auto& dpi = session.DPI;

    const uint8_t colour = BoundBoxDebugColours[EnumValue(ps->InteractionItem)];
    const uint8_t rotation = session.CurrentRotation;

    const CoordsXYZ frontTop = {
        ps->Bounds.x_end,
        ps->Bounds.y_end,
        ps->Bounds.z_end,
    };
    const auto screenCoordFrontTop = Translate3DTo2DWithZ(rotation, frontTop);

    const CoordsXYZ frontBottom = {
        ps->Bounds.x_end,
        ps->Bounds.y_end,
        ps->Bounds.z,
    };
    const auto screenCoordFrontBottom = Translate3DTo2DWithZ(rotation, frontBottom);

    const CoordsXYZ leftTop = {
        ps->Bounds.x,
        ps->Bounds.y_end,
        ps->Bounds.z_end,
    };
    const auto screenCoordLeftTop = Translate3DTo2DWithZ(rotation, leftTop);

    const CoordsXYZ leftBottom = {
        ps->Bounds.x,
        ps->Bounds.y_end,
        ps->Bounds.z,
    };
    const auto screenCoordLeftBottom = Translate3DTo2DWithZ(rotation, leftBottom);

    const CoordsXYZ rightTop = {
        ps->Bounds.x_end,
        ps->Bounds.y,
        ps->Bounds.z_end,
    };
    const auto screenCoordRightTop = Translate3DTo2DWithZ(rotation, rightTop);

    const CoordsXYZ rightBottom = {
        ps->Bounds.x_end,
        ps->Bounds.y,
        ps->Bounds.z,
    };
    const auto screenCoordRightBottom = Translate3DTo2DWithZ(rotation, rightBottom);

    const CoordsXYZ backTop = {
        ps->Bounds.x,
        ps->Bounds.y,
        ps->Bounds.z_end,
    };
    const auto screenCoordBackTop = Translate3DTo2DWithZ(rotation, backTop);

    const CoordsXYZ backBottom = {
        ps->Bounds.x,
        ps->Bounds.y,
        ps->Bounds.z,
    };
    const auto screenCoordBackBottom = Translate3DTo2DWithZ(rotation, backBottom);

    // bottom square
    GfxDrawLine(dpi, { screenCoordFrontBottom, screenCoordLeftBottom }, colour);
    GfxDrawLine(dpi, { screenCoordBackBottom, screenCoordLeftBottom }, colour);
    GfxDrawLine(dpi, { screenCoordBackBottom, screenCoordRightBottom }, colour);
    GfxDrawLine(dpi, { screenCoordFrontBottom, screenCoordRightBottom }, colour);

    // vertical back + sides
    GfxDrawLine(dpi, { screenCoordBackTop, screenCoordBackBottom }, colour);
    GfxDrawLine(dpi, { screenCoordLeftTop, screenCoordLeftBottom }, colour);
    GfxDrawLine(dpi, { screenCoordRightTop, screenCoordRightBottom }, colour);

    // top square back
    GfxDrawLine(dpi, { screenCoordBackTop, screenCoordLeftTop }, colour);
    GfxDrawLine(dpi, { screenCoordBackTop, screenCoordRightTop }, colour);

    GfxDrawSprite(dpi, imageId, { x, y });

    // vertical front
    GfxDrawLine(dpi, { screenCoordFrontTop, screenCoordFrontBottom }, colour);

    // top square
    GfxDrawLine(dpi, { screenCoordFrontTop, screenCoordLeftTop }, colour);
    GfxDrawLine(dpi, { screenCoordFrontTop, screenCoordRightTop }, colour);
}

static ImageId PaintPSColourifyImage(const PaintStruct* ps, ImageId imageId, uint32_t viewFlags)
{
    auto visibility = GetPaintStructVisibility(ps, viewFlags);
    switch (visibility)
    {
        case VisibilityKind::Partial:
            return imageId.WithTransparency(FilterPaletteID::PaletteDarken1);
        case VisibilityKind::Hidden:
            return ImageId();
        default:
            return imageId;
    }
}

PaintSession* PaintSessionAlloc(DrawPixelInfo& dpi, uint32_t viewFlags, uint8_t rotation)
{
    return GetContext()->GetPainter()->CreateSession(dpi, viewFlags, rotation);
}

void PaintSessionFree(PaintSession* session)
{
    GetContext()->GetPainter()->ReleaseSession(session);
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
 * @return (ebp) PaintStruct on success (CF == 0), nullptr on failure (CF == 1)
 */
// Track Pieces, Shops.
PaintStruct* PaintAddImageAsParent(
    PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    session.LastPS = nullptr;
    session.LastAttachedPS = nullptr;

    auto* ps = CreateNormalPaintStruct(session, image_id, offset, boundBox);
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
 * @return (ebp) PaintStruct on success (CF == 0), nullptr on failure (CF == 1)
 * Creates a paint struct but does not allocate to a paint quadrant. Result cannot be ignored!
 */
[[nodiscard]] PaintStruct* PaintAddImageAsOrphan(
    PaintSession& session, const ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    session.LastPS = nullptr;
    session.LastAttachedPS = nullptr;
    return CreateNormalPaintStruct(session, imageId, offset, boundBox);
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
 * @return (ebp) PaintStruct on success (CF == 0), nullptr on failure (CF == 1)
 * If there is no parent paint struct then image is added as a parent
 */
PaintStruct* PaintAddImageAsChild(
    PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    PaintStruct* parentPS = session.LastPS;
    if (parentPS == nullptr)
    {
        return PaintAddImageAsParent(session, image_id, offset, boundBox);
    }

    auto* ps = CreateNormalPaintStruct(session, image_id, offset, boundBox);
    if (ps == nullptr)
    {
        return nullptr;
    }

    parentPS->Children = ps;

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
bool PaintAttachToPreviousAttach(PaintSession& session, const ImageId imageId, int32_t x, int32_t y)
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
    ps->RelativePos = { x, y };
    ps->IsMasked = false;
    ps->NextEntry = nullptr;

    previousAttachedPS->NextEntry = ps;

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
bool PaintAttachToPreviousPS(PaintSession& session, const ImageId image_id, int32_t x, int32_t y)
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
    ps->RelativePos = { x, y };
    ps->IsMasked = false;

    AttachedPaintStruct* oldFirstAttached = masterPs->Attached;
    masterPs->Attached = ps;
    ps->NextEntry = oldFirstAttached;

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
    PaintSession& session, money64 amount, StringId string_id, int32_t y, int32_t z, int8_t y_offsets[], int32_t offset_x,
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
    const auto coord = Translate3DTo2DWithZ(rotation, position);

    ps->string_id = string_id;
    ps->NextEntry = nullptr;
    std::memcpy(ps->args, &amount, sizeof(amount));
    ps->args[2] = 0;
    ps->args[3] = 0;
    ps->y_offsets = reinterpret_cast<uint8_t*>(y_offsets);
    ps->ScreenPos = ScreenCoordsXY{ coord.x + offset_x, coord.y };
}

/**
 *
 *  rct2: 0x006860C3
 */
void PaintDrawMoneyStructs(DrawPixelInfo& dpi, PaintStringStruct* ps)
{
    do
    {
        char buffer[256]{};
        FormatStringLegacy(buffer, sizeof(buffer), ps->string_id, &ps->args);

        // Use sprite font unless the currency contains characters unsupported by the sprite font
        auto forceSpriteFont = false;
        const auto& currencyDesc = CurrencyDescriptors[EnumValue(Config::Get().general.CurrencyFormat)];
        if (LocalisationService_UseTrueTypeFont() && FontSupportsStringSprite(currencyDesc.symbol_unicode))
        {
            forceSpriteFont = true;
        }

        GfxDrawStringWithYOffsets(
            dpi, buffer, { COLOUR_BLACK }, ps->ScreenPos, reinterpret_cast<int8_t*>(ps->y_offsets), forceSpriteFont,
            FontStyle::Medium);
    } while ((ps = ps->NextEntry) != nullptr);
}
