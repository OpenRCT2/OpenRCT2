/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../drawing/Drawing.String.h"
#include "../drawing/Drawing.h"
#include "../drawing/Font.h"
#include "../interface/Viewport.h"
#include "../localisation/Currency.h"
#include "../localisation/Formatting.h"
#include "../localisation/LocalisationService.h"
#include "../localisation/StringIdType.h"
#include "../paint/Painter.h"
#include "../profiling/Profiling.h"
#include "Boundbox.h"
#include "Paint.Entity.h"
#include "tile_element/Paint.TileElement.h"

#include <algorithm>
#include <array>
#include <cassert>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Numerics;

// Globals for paint clipping
uint8_t gClipHeight = 128; // Default to middle value
CoordsXY gClipSelectionA = { 0, 0 };
CoordsXY gClipSelectionB = { kMaximumTileStartXY, kMaximumTileStartXY };

static constexpr PaletteIndex kBoundBoxDebugColours[] = {
    PaletteIndex::transparent, // NONE
    PaletteIndex::pi102,       // TERRAIN
    PaletteIndex::pi114,       // SPRITE
    PaletteIndex::pi229,       // RIDE
    PaletteIndex::pi126,       // WATER
    PaletteIndex::pi138,       // SCENERY
    PaletteIndex::pi150,       // FOOTPATH
    PaletteIndex::pi162,       // FOOTPATH_ITEM
    PaletteIndex::pi174,       // PARK
    PaletteIndex::pi186,       // WALL
    PaletteIndex::pi198,       // LARGE_SCENERY
    PaletteIndex::hotPink8,    // LABEL
    PaletteIndex::pi222,       // BANNER
};

bool gShowDirtyVisuals;
bool gPaintBoundingBoxes;
bool gPaintBlockedTiles;
bool gPaintStableSort;

static void paintPSImageWithBoundingBoxes(PaintSession& session, PaintStruct* ps, ImageId imageId, int32_t x, int32_t y);
static ImageId paintPSColourifyImage(const PaintStruct* ps, ImageId imageId, uint32_t viewFlags);

static int32_t remapPositionToQuadrant(const PaintStruct& ps, uint8_t rotation)
{
    constexpr auto MapRangeMax = MaxPaintQuadrants * kCoordsXYStep;
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

static void paintSessionAddPSToQuadrant(PaintSession& session, PaintStruct* ps)
{
    const auto positionHash = remapPositionToQuadrant(*ps, session.currentRotation);

    // Values below zero or above MaxPaintQuadrants are void, corners also share the same quadrant as void.
    const uint32_t paintQuadrantIndex = std::clamp(positionHash / kCoordsXYStep, 0, MaxPaintQuadrants - 1);

    ps->quadrantIndex = paintQuadrantIndex;
    ps->nextQuadrantEntry = session.quadrants[paintQuadrantIndex];
    session.quadrants[paintQuadrantIndex] = ps;

    session.quadrantBackIndex = std::min(session.quadrantBackIndex, paintQuadrantIndex);
    session.quadrantFrontIndex = std::max(session.quadrantFrontIndex, paintQuadrantIndex);
}

static constexpr bool imageWithinRT(const ScreenCoordsXY& imagePos, const G1Element& g1, const RenderTarget& rt)
{
    const int32_t left = imagePos.x + g1.xOffset;
    const int32_t bottom = imagePos.y + g1.yOffset;

    const int32_t right = left + g1.width;
    const int32_t top = bottom + g1.height;

    // mber: It is possible to use only the bottom else block here if you change <= and >= to simply < and >.
    // However, since this is used to cull paint structs, I'd prefer to keep the condition strict and calculate
    // the culling differently for minifying and magnifying.
    const auto zoom = rt.zoom_level;
    if (zoom > ZoomLevel{ 0 })
    {
        const int32_t x = zoom.ApplyTo(rt.cullingX);
        const int32_t y = zoom.ApplyTo(rt.cullingY);
        if (right <= x)
            return false;
        if (top <= y)
            return false;
        if (left >= x + zoom.ApplyTo(rt.cullingWidth))
            return false;
        if (bottom >= y + zoom.ApplyTo(rt.cullingHeight))
            return false;
    }
    else
    {
        if (zoom.ApplyInversedTo(right) <= rt.cullingX)
            return false;
        if (zoom.ApplyInversedTo(top) <= rt.cullingY)
            return false;
        if (zoom.ApplyInversedTo(left) >= rt.cullingX + rt.cullingWidth)
            return false;
        if (zoom.ApplyInversedTo(bottom) >= rt.cullingY + rt.cullingHeight)
            return false;
    }
    return true;
}

static constexpr CoordsXYZ rotateBoundBoxSize(const CoordsXYZ& bbSize, const uint8_t rotation)
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
static PaintStruct* createNormalPaintStruct(
    PaintSession& session, ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    auto* const g1 = GfxGetG1Element(imageId);
    if (g1 == nullptr)
    {
        return nullptr;
    }

    const auto swappedRotation = DirectionFlipXAxis(session.currentRotation);
    auto swappedRotCoord = CoordsXYZ{ offset.Rotate(swappedRotation), offset.z };
    swappedRotCoord += session.spritePosition;

    const auto imagePos = Translate3DTo2DWithZ(session.currentRotation, swappedRotCoord);

    if (!imageWithinRT(imagePos, *g1, session.rt))
    {
        return nullptr;
    }

    const auto rotBoundBoxOffset = CoordsXYZ{ boundBox.offset.Rotate(swappedRotation), boundBox.offset.z };
    const auto rotBoundBoxSize = rotateBoundBoxSize(boundBox.length, session.currentRotation);

    auto* ps = session.allocateNormalPaintEntry();
    if (ps == nullptr)
    {
        return nullptr;
    }

    ps->imageId = imageId;
    ps->screenPos = imagePos;
    ps->bounds.xEnd = rotBoundBoxSize.x + rotBoundBoxOffset.x + session.spritePosition.x;
    ps->bounds.yEnd = rotBoundBoxSize.y + rotBoundBoxOffset.y + session.spritePosition.y;
    ps->bounds.zEnd = rotBoundBoxSize.z + rotBoundBoxOffset.z;
    ps->bounds.x = rotBoundBoxOffset.x + session.spritePosition.x;
    ps->bounds.y = rotBoundBoxOffset.y + session.spritePosition.y;
    ps->bounds.z = rotBoundBoxOffset.z;
    ps->attached = nullptr;
    ps->children = nullptr;
    ps->nextQuadrantEntry = nullptr;
    ps->interactionItem = session.interactionType;
    ps->mapPos = session.mapPosition;
    ps->element = session.currentlyDrawnTileElement;
    ps->entity = session.currentlyDrawnEntity;

    return ps;
}

static PaintStruct* createNormalPaintStructHeight(
    PaintSession& session, const ImageId imageId, const int32_t height, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    auto* const g1 = GfxGetG1Element(imageId);
    if (g1 == nullptr)
    {
        return nullptr;
    }

    const auto swappedRotation = DirectionFlipXAxis(session.currentRotation);
    auto swappedRotCoord = CoordsXYZ{ offset.Rotate(swappedRotation), offset.z + height };
    swappedRotCoord += session.spritePosition;

    const auto imagePos = Translate3DTo2DWithZ(session.currentRotation, swappedRotCoord);

    if (!imageWithinRT(imagePos, *g1, session.rt))
    {
        return nullptr;
    }

    const auto rotBoundBoxOffset = CoordsXYZ{ boundBox.offset.Rotate(swappedRotation), boundBox.offset.z + height };
    const auto rotBoundBoxSize = rotateBoundBoxSize(boundBox.length, session.currentRotation);

    auto* ps = session.allocateNormalPaintEntry();
    if (ps == nullptr)
    {
        return nullptr;
    }

    ps->imageId = imageId;
    ps->screenPos = imagePos;
    ps->bounds.xEnd = rotBoundBoxSize.x + rotBoundBoxOffset.x + session.spritePosition.x;
    ps->bounds.yEnd = rotBoundBoxSize.y + rotBoundBoxOffset.y + session.spritePosition.y;
    ps->bounds.zEnd = rotBoundBoxSize.z + rotBoundBoxOffset.z;
    ps->bounds.x = rotBoundBoxOffset.x + session.spritePosition.x;
    ps->bounds.y = rotBoundBoxOffset.y + session.spritePosition.y;
    ps->bounds.z = rotBoundBoxOffset.z;
    ps->attached = nullptr;
    ps->children = nullptr;
    ps->nextQuadrantEntry = nullptr;
    ps->interactionItem = session.interactionType;
    ps->mapPos = session.mapPosition;
    ps->element = session.currentlyDrawnTileElement;
    ps->entity = session.currentlyDrawnEntity;

    return ps;
}

template<uint8_t direction>
void paintSessionGenerateRotate(PaintSession& session)
{
    // Optimised modified version of ViewportPosToMapPos
    ScreenCoordsXY screenCoord = { floor2(session.rt.WorldX(), 32), floor2((session.rt.WorldY() - 16), 32) };
    CoordsXY mapTile = { screenCoord.y - screenCoord.x / 2, screenCoord.y + screenCoord.x / 2 };
    mapTile = mapTile.Rotate(direction);

    if constexpr (direction & 1)
    {
        mapTile.y -= 16;
    }
    mapTile = mapTile.ToTileStart();

    uint16_t numVerticalTiles = (session.rt.WorldHeight() + 2128) >> 5;

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
void paintSessionGenerate(PaintSession& session)
{
    switch (DirectionFlipXAxis(session.currentRotation))
    {
        case 0:
            paintSessionGenerateRotate<0>(session);
            break;
        case 1:
            paintSessionGenerateRotate<1>(session);
            break;
        case 2:
            paintSessionGenerateRotate<2>(session);
            break;
        case 3:
            paintSessionGenerateRotate<3>(session);
            break;
    }
}

template<uint8_t TRotation>
static bool checkBoundingBox(const PaintStructBoundBox& initialBBox, const PaintStructBoundBox& currentBBox)
{
    if constexpr (TRotation == 0)
    {
        if (initialBBox.zEnd >= currentBBox.z && initialBBox.yEnd >= currentBBox.y && initialBBox.xEnd >= currentBBox.x
            && !(initialBBox.z < currentBBox.zEnd && initialBBox.y < currentBBox.yEnd && initialBBox.x < currentBBox.xEnd))
        {
            return true;
        }
    }
    else if constexpr (TRotation == 1)
    {
        if (initialBBox.zEnd >= currentBBox.z && initialBBox.yEnd >= currentBBox.y && initialBBox.xEnd < currentBBox.x
            && !(initialBBox.z < currentBBox.zEnd && initialBBox.y < currentBBox.yEnd && initialBBox.x >= currentBBox.xEnd))
        {
            return true;
        }
    }
    else if constexpr (TRotation == 2)
    {
        if (initialBBox.zEnd >= currentBBox.z && initialBBox.yEnd < currentBBox.y && initialBBox.xEnd < currentBBox.x
            && !(initialBBox.z < currentBBox.zEnd && initialBBox.y >= currentBBox.yEnd && initialBBox.x >= currentBBox.xEnd))
        {
            return true;
        }
    }
    else if constexpr (TRotation == 3)
    {
        if (initialBBox.zEnd >= currentBBox.z && initialBBox.yEnd < currentBBox.y && initialBBox.xEnd >= currentBBox.x
            && !(initialBBox.z < currentBBox.zEnd && initialBBox.y >= currentBBox.yEnd && initialBBox.x < currentBBox.xEnd))
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

static PaintStruct* paintStructsFirstInQuadrant(PaintStruct* psNext, uint16_t quadrantIndex)
{
    PaintStruct* ps;
    do
    {
        ps = psNext;
        psNext = psNext->nextQuadrantEntry;
        if (psNext == nullptr)
            return ps;
    } while (quadrantIndex > psNext->quadrantIndex);
    return ps;
}

// Initializes sorting flags for all entries in the specified quadrant by quadrantIndex.
// Sorting flags specify whether a node needs to be traversed, is a neighbour, or is outside the
// quadrant range.
static void paintStructsInitializeSort(PaintStruct* ps, uint16_t quadrantIndex, uint8_t flag)
{
    do
    {
        ps = ps->nextQuadrantEntry;
        if (ps == nullptr)
            break;

        if (ps->quadrantIndex > quadrantIndex + 1)
        {
            // Outside of the range.
            ps->sortFlags = PaintSortFlags::OutsideQuadrant;
        }
        else if (ps->quadrantIndex == quadrantIndex + 1)
        {
            // Is neighbour and requires a visit.
            ps->sortFlags = PaintSortFlags::Neighbour | PaintSortFlags::PendingVisit;
        }
        else if (ps->quadrantIndex == quadrantIndex)
        {
            // In specified quadrant, requires visit.
            ps->sortFlags = flag | PaintSortFlags::PendingVisit;
        }
    } while (ps->quadrantIndex <= quadrantIndex + 1);
}

// Returns a pair of parent and child where child is the next node that requires traversal.
// Because this structure uses a singly linked list we need to keep track of the parent in order
// to be able to re-order the list.
static std::pair<PaintStruct*, PaintStruct*> paintStructsGetNextPending(PaintStruct* ps)
{
    PaintStruct* ps_next;
    while (true)
    {
        ps_next = ps->nextQuadrantEntry;
        if (ps_next == nullptr)
        {
            // End of the current list.
            return { nullptr, nullptr };
        }
        if (ps_next->sortFlags & PaintSortFlags::OutsideQuadrant)
        {
            // Reached point outside of specified quadrant.
            return { nullptr, nullptr };
        }
        if (ps_next->sortFlags & PaintSortFlags::PendingVisit)
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
static void paintStructsSortQuadrantLegacy(PaintStruct* parent, PaintStruct* child)
{
    // Mark visited.
    child->sortFlags &= ~PaintSortFlags::PendingVisit;

    // Compare all the children below the first child and move them up in the list if they intersect.
    const PaintStructBoundBox& initialBBox = child->bounds;

    for (;;)
    {
        auto* ps = child;
        child = child->nextQuadrantEntry;

        if (child == nullptr || child->sortFlags & PaintSortFlags::OutsideQuadrant)
        {
            break;
        }

        if (!(child->sortFlags & PaintSortFlags::Neighbour))
        {
            continue;
        }

        if (checkBoundingBox<TRotation>(initialBBox, child->bounds))
        {
            // Child node intersects with current node, move behind.
            ps->nextQuadrantEntry = child->nextQuadrantEntry;

            auto* psTemp = parent->nextQuadrantEntry;
            parent->nextQuadrantEntry = child;

            child->nextQuadrantEntry = psTemp;
            child = ps;
        }
    }
}

// Re-orders all nodes after the specified child node and marks the child node as traversed. The resulting
// order of the children is the depth based on rotation and dimensions of the bounding box.
template<uint8_t TRotation>
static void paintStructsSortQuadrantStable(PaintStruct* parent, PaintStruct* child)
{
    // Mark visited.
    child->sortFlags &= ~PaintSortFlags::PendingVisit;

    // Compare all the children below the first child and move them up in the list if they intersect.
    const PaintStructBoundBox& initialBBox = child->bounds;

    // Create a temporary list to collect sorted nodes in stable order.
    PaintStruct* sortedHead = nullptr;
    PaintStruct* sortedTail = nullptr;

    // Traverse the list and reorder based on intersection.
    for (;;)
    {
        PaintStruct* next = child->nextQuadrantEntry;

        // Stop if at the end of the list or outside the quadrant range.
        if (next == nullptr || next->sortFlags & PaintSortFlags::OutsideQuadrant)
        {
            break;
        }

        // Ignore nodes that are not neighbors.
        if (!(next->sortFlags & PaintSortFlags::Neighbour))
        {
            child = next;
            continue;
        }

        // Detach the current node from the list if it intersects.
        if (checkBoundingBox<TRotation>(initialBBox, next->bounds))
        {
            child->nextQuadrantEntry = next->nextQuadrantEntry;

            if (sortedHead == nullptr)
            {
                sortedHead = next;
                sortedTail = next;
                next->nextQuadrantEntry = nullptr;
            }
            else
            {
                sortedTail->nextQuadrantEntry = next;
                sortedTail = next;
                next->nextQuadrantEntry = nullptr;
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
        PaintStruct* originalNext = parent->nextQuadrantEntry;
        parent->nextQuadrantEntry = sortedHead;
        sortedTail->nextQuadrantEntry = originalNext;
    }
}

template<bool TStableSort, uint8_t TRotation>
static PaintStruct* paintArrangeStructsHelperRotation(PaintStruct* psQuadrantEntry, uint16_t quadrantIndex, uint8_t flag)
{
    // We keep track of the first node in the quadrant so the next call with a higher quadrant index
    // can use this node to skip some iterations.
    psQuadrantEntry = paintStructsFirstInQuadrant(psQuadrantEntry, quadrantIndex);

    // Visit all nodes in the linked quadrant list and determine their current
    // sorting relevancy.
    paintStructsInitializeSort(psQuadrantEntry, quadrantIndex, flag);

    // Iterate all nodes in the current list and re-order them based on
    // the current rotation and their bounding box.
    for (auto* ps = psQuadrantEntry; ps != nullptr;)
    {
        const auto [parent, child] = paintStructsGetNextPending(ps);
        if (parent == nullptr)
        {
            break;
        }

        if constexpr (TStableSort)
        {
            paintStructsSortQuadrantStable<TRotation>(parent, child);
        }
        else
        {
            paintStructsSortQuadrantLegacy<TRotation>(parent, child);
        }

        ps = parent;
    }

    return psQuadrantEntry;
}

// Iterates over all the quadrant lists and links them together as a
// singly linked list.
// The paint session has a head member which is the first entry.
static void paintStructsLinkQuadrants(PaintSessionCore& session, PaintStruct& psHead)
{
    PaintStruct* ps = &psHead;
    ps->nextQuadrantEntry = nullptr;

    uint32_t quadrantIndex = session.quadrantBackIndex;
    do
    {
        PaintStruct* psNext = session.quadrants[quadrantIndex];
        if (psNext != nullptr)
        {
            ps->nextQuadrantEntry = psNext;
            do
            {
                ps = psNext;
                psNext = psNext->nextQuadrantEntry;

            } while (psNext != nullptr);
        }
    } while (++quadrantIndex <= session.quadrantFrontIndex);
}

template<bool TStableSort, int TRotation>
static void paintSessionArrangeImpl(PaintSessionCore& session)
{
    uint32_t quadrantIndex = session.quadrantBackIndex;
    if (quadrantIndex == UINT32_MAX)
    {
        return;
    }

    // psHead is an intermediate node that is used to link all the quadrant lists together,
    // this was previously stored in PaintSession but only the NextQuadrantEntry is relevant here.
    // The head node is not part of the linked list and just serves as an entry point.
    PaintStruct psHead{};
    paintStructsLinkQuadrants(session, psHead);

    PaintStruct* psNextQuadrant = paintArrangeStructsHelperRotation<TStableSort, TRotation>(
        &psHead, session.quadrantBackIndex, PaintSortFlags::Neighbour);

    while (++quadrantIndex < session.quadrantFrontIndex)
    {
        psNextQuadrant = paintArrangeStructsHelperRotation<TStableSort, TRotation>(
            psNextQuadrant, quadrantIndex, PaintSortFlags::None);
    }

    session.paintHead = psHead.nextQuadrantEntry;
}

using PaintArrangeWithRotation = void (*)(PaintSessionCore& session);

constexpr std::array _paintArrangeFuncsLegacy = {
    paintSessionArrangeImpl<false, 0>,
    paintSessionArrangeImpl<false, 1>,
    paintSessionArrangeImpl<false, 2>,
    paintSessionArrangeImpl<false, 3>,
};

constexpr std::array _paintArrangeFuncsStable = {
    paintSessionArrangeImpl<true, 0>,
    paintSessionArrangeImpl<true, 1>,
    paintSessionArrangeImpl<true, 2>,
    paintSessionArrangeImpl<true, 3>,
};

/**
 *
 *  rct2: 0x00688217
 */
void paintSessionArrange(PaintSessionCore& session)
{
    PROFILED_FUNCTION();
    if (gPaintStableSort)
    {
        return _paintArrangeFuncsStable[session.currentRotation](session);
    }
    return _paintArrangeFuncsLegacy[session.currentRotation](session);
}

static inline void paintAttachedPS(RenderTarget& rt, PaintStruct* ps, uint32_t viewFlags)
{
    AttachedPaintStruct* attached_ps = ps->attached;
    for (; attached_ps != nullptr; attached_ps = attached_ps->nextEntry)
    {
        const auto screenCoords = ps->screenPos + attached_ps->relativePos;

        auto imageId = paintPSColourifyImage(ps, attached_ps->imageId, viewFlags);
        if (attached_ps->isMasked)
        {
            GfxDrawSpriteRawMasked(rt, screenCoords, imageId, attached_ps->colourImageId);
        }
        else
        {
            GfxDrawSprite(rt, imageId, screenCoords);
        }
    }
}

static inline void paintDrawStruct(PaintSession& session, PaintStruct* ps)
{
    auto screenPos = ps->screenPos;
    if (ps->interactionItem == ViewportInteractionItem::entity)
    {
        if (session.rt.zoom_level >= ZoomLevel{ 1 })
        {
            screenPos.x = floor2(screenPos.x, 2);
            screenPos.y = floor2(screenPos.y, 2);
            if (session.rt.zoom_level >= ZoomLevel{ 2 })
            {
                screenPos.x = floor2(screenPos.x, 4);
                screenPos.y = floor2(screenPos.y, 4);
            }
        }
    }
    auto imageId = paintPSColourifyImage(ps, ps->imageId, session.viewFlags);
    if (gPaintBoundingBoxes)
    {
        paintPSImageWithBoundingBoxes(session, ps, imageId, screenPos.x, screenPos.y);
    }
    else
    {
        GfxDrawSprite(session.rt, imageId, screenPos);
    }

    if (ps->children != nullptr)
    {
        paintDrawStruct(session, ps->children);
    }
    else
    {
        paintAttachedPS(session.rt, ps, session.viewFlags);
    }
}

/**
 *
 *  rct2: 0x00688485
 */
void paintDrawStructs(PaintSession& session)
{
    PROFILED_FUNCTION();

    for (PaintStruct* ps = session.paintHead; ps != nullptr; ps = ps->nextQuadrantEntry)
    {
        paintDrawStruct(session, ps);
    }
}

static void paintPSImageWithBoundingBoxes(PaintSession& session, PaintStruct* ps, ImageId imageId, int32_t x, int32_t y)
{
    auto& rt = session.rt;

    const PaletteIndex colour = kBoundBoxDebugColours[EnumValue(ps->interactionItem)];
    const uint8_t rotation = session.currentRotation;

    const CoordsXYZ frontTop = {
        ps->bounds.xEnd,
        ps->bounds.yEnd,
        ps->bounds.zEnd,
    };
    const auto screenCoordFrontTop = Translate3DTo2DWithZ(rotation, frontTop);

    const CoordsXYZ frontBottom = {
        ps->bounds.xEnd,
        ps->bounds.yEnd,
        ps->bounds.z,
    };
    const auto screenCoordFrontBottom = Translate3DTo2DWithZ(rotation, frontBottom);

    const CoordsXYZ leftTop = {
        ps->bounds.x,
        ps->bounds.yEnd,
        ps->bounds.zEnd,
    };
    const auto screenCoordLeftTop = Translate3DTo2DWithZ(rotation, leftTop);

    const CoordsXYZ leftBottom = {
        ps->bounds.x,
        ps->bounds.yEnd,
        ps->bounds.z,
    };
    const auto screenCoordLeftBottom = Translate3DTo2DWithZ(rotation, leftBottom);

    const CoordsXYZ rightTop = {
        ps->bounds.xEnd,
        ps->bounds.y,
        ps->bounds.zEnd,
    };
    const auto screenCoordRightTop = Translate3DTo2DWithZ(rotation, rightTop);

    const CoordsXYZ rightBottom = {
        ps->bounds.xEnd,
        ps->bounds.y,
        ps->bounds.z,
    };
    const auto screenCoordRightBottom = Translate3DTo2DWithZ(rotation, rightBottom);

    const CoordsXYZ backTop = {
        ps->bounds.x,
        ps->bounds.y,
        ps->bounds.zEnd,
    };
    const auto screenCoordBackTop = Translate3DTo2DWithZ(rotation, backTop);

    const CoordsXYZ backBottom = {
        ps->bounds.x,
        ps->bounds.y,
        ps->bounds.z,
    };
    const auto screenCoordBackBottom = Translate3DTo2DWithZ(rotation, backBottom);

    // bottom square
    GfxDrawLine(rt, { screenCoordFrontBottom, screenCoordLeftBottom }, colour);
    GfxDrawLine(rt, { screenCoordBackBottom, screenCoordLeftBottom }, colour);
    GfxDrawLine(rt, { screenCoordBackBottom, screenCoordRightBottom }, colour);
    GfxDrawLine(rt, { screenCoordFrontBottom, screenCoordRightBottom }, colour);

    // vertical back + sides
    GfxDrawLine(rt, { screenCoordBackTop, screenCoordBackBottom }, colour);
    GfxDrawLine(rt, { screenCoordLeftTop, screenCoordLeftBottom }, colour);
    GfxDrawLine(rt, { screenCoordRightTop, screenCoordRightBottom }, colour);

    // top square back
    GfxDrawLine(rt, { screenCoordBackTop, screenCoordLeftTop }, colour);
    GfxDrawLine(rt, { screenCoordBackTop, screenCoordRightTop }, colour);

    GfxDrawSprite(rt, imageId, { x, y });

    // vertical front
    GfxDrawLine(rt, { screenCoordFrontTop, screenCoordFrontBottom }, colour);

    // top square
    GfxDrawLine(rt, { screenCoordFrontTop, screenCoordLeftTop }, colour);
    GfxDrawLine(rt, { screenCoordFrontTop, screenCoordRightTop }, colour);
}

static ImageId paintPSColourifyImage(const PaintStruct* ps, ImageId imageId, uint32_t viewFlags)
{
    auto visibility = GetPaintStructVisibility(ps, viewFlags);
    switch (visibility)
    {
        case VisibilityKind::partial:
            return imageId.WithTransparency(FilterPaletteID::paletteDarken1);
        case VisibilityKind::hidden:
            return ImageId();
        default:
            return imageId;
    }
}

PaintSession* paintSessionAlloc(RenderTarget& rt, uint32_t viewFlags, uint8_t rotation)
{
    return GetContext()->GetPainter()->CreateSession(rt, viewFlags, rotation);
}

void paintSessionFree(PaintSession* session)
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
PaintStruct* paintAddImageAsParent(
    PaintSession& session, const ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    session.lastPS = nullptr;
    session.lastAttachedPS = nullptr;

    auto* ps = createNormalPaintStruct(session, imageId, offset, boundBox);
    if (ps == nullptr)
    {
        return nullptr;
    }

    paintSessionAddPSToQuadrant(session, ps);

    return ps;
}

/**
 *
 *  rct2: 0x00686EF0, 0x00687056, 0x006871C8, 0x0068733C, 0x0098198C
 *
 * @param imageId (ebx)
 * @param xOffset (al)
 * @param yOffset (cl)
 * @param boundBoxLengthX (di)
 * @param boundBoxLengthY (si)
 * @param boundBoxLengthZ (ah)
 * @param zOffset (dx)
 * @param boundBoxOffsetX (0x009DEA52)
 * @param boundBoxOffsetY (0x009DEA54)
 * @param boundBoxOffsetZ (0x009DEA56)
 * @return (ebp) PaintStruct on success (CF == 0), nullptr on failure (CF == 1)
 * Creates a paint struct but does not allocate to a paint quadrant. Result cannot be ignored!
 */
[[nodiscard]] PaintStruct* paintAddImageAsOrphan(
    PaintSession& session, const ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    session.lastPS = nullptr;
    session.lastAttachedPS = nullptr;
    return createNormalPaintStruct(session, imageId, offset, boundBox);
}

/**
 *
 *  rct2: 0x006874B0, 0x00687618, 0x0068778C, 0x00687902, 0x0098199C
 *
 * @param image_id (ebx)
 * @param xOffset (al)
 * @param yOffset (cl)
 * @param boundBoxLengthX (di)
 * @param boundBoxLengthY (si)
 * @param boundBoxLengthZ (ah)
 * @param zOffset (dx)
 * @param boundBoxOffsetX (0x009DEA52)
 * @param boundBoxOffsetY (0x009DEA54)
 * @param boundBoxOffsetZ (0x009DEA56)
 * @return (ebp) PaintStruct on success (CF == 0), nullptr on failure (CF == 1)
 * If there is no parent paint struct then image is added as a parent
 */
PaintStruct* paintAddImageAsChild(
    PaintSession& session, const ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    PaintStruct* parentPS = session.lastPS;
    if (parentPS == nullptr)
    {
        return paintAddImageAsParent(session, imageId, offset, boundBox);
    }

    auto* ps = createNormalPaintStruct(session, imageId, offset, boundBox);
    if (ps == nullptr)
    {
        return nullptr;
    }

    parentPS->children = ps;

    return ps;
}

PaintStruct* paintAddImageAsParentHeight(
    PaintSession& session, const ImageId imageId, const int32_t height, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    session.lastPS = nullptr;
    session.lastAttachedPS = nullptr;

    auto* const ps = createNormalPaintStructHeight(session, imageId, height, offset, boundBox);
    if (ps == nullptr)
    {
        return nullptr;
    }

    paintSessionAddPSToQuadrant(session, ps);

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
bool paintAttachToPreviousAttach(PaintSession& session, const ImageId imageId, int32_t x, int32_t y)
{
    auto* previousAttachedPS = session.lastAttachedPS;
    if (previousAttachedPS == nullptr)
    {
        return paintAttachToPreviousPS(session, imageId, x, y);
    }

    auto* ps = session.allocateAttachedPaintEntry();
    if (ps == nullptr)
    {
        return false;
    }

    ps->imageId = imageId;
    ps->relativePos = { x, y };
    ps->isMasked = false;
    ps->nextEntry = nullptr;

    previousAttachedPS->nextEntry = ps;

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
bool paintAttachToPreviousPS(PaintSession& session, const ImageId imageId, int32_t x, int32_t y)
{
    auto* masterPs = session.lastPS;
    if (masterPs == nullptr)
    {
        return false;
    }

    auto* ps = session.allocateAttachedPaintEntry();
    if (ps == nullptr)
    {
        return false;
    }

    ps->imageId = imageId;
    ps->relativePos = { x, y };
    ps->isMasked = false;

    AttachedPaintStruct* oldFirstAttached = masterPs->attached;
    masterPs->attached = ps;
    ps->nextEntry = oldFirstAttached;

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
void paintFloatingMoneyEffect(
    PaintSession& session, money64 amount, StringId stringId, int32_t y, int32_t z, int8_t yOffsets[], int32_t offsetX,
    uint32_t rotation)
{
    auto* ps = session.allocateStringPaintEntry();
    if (ps == nullptr)
    {
        return;
    }

    const CoordsXYZ position = {
        session.spritePosition.x,
        session.spritePosition.y,
        z,
    };
    const auto coord = Translate3DTo2DWithZ(rotation, position);

    ps->stringId = stringId;
    ps->nextEntry = nullptr;
    std::memcpy(ps->args, &amount, sizeof(amount));
    ps->args[2] = 0;
    ps->args[3] = 0;
    ps->yOffsets = reinterpret_cast<uint8_t*>(yOffsets);
    ps->screenPos = ScreenCoordsXY{ coord.x + offsetX, coord.y };
}

/**
 *
 *  rct2: 0x006860C3
 */
void paintDrawMoneyStructs(RenderTarget& rt, PaintStringStruct* ps)
{
    do
    {
        char buffer[256]{};
        FormatStringLegacy(buffer, sizeof(buffer), ps->stringId, &ps->args);

        // Use sprite font unless the currency contains characters unsupported by the sprite font
        auto forceSpriteFont = false;
        const auto& currencyDesc = CurrencyDescriptors[EnumValue(Config::Get().general.currencyFormat)];
        if (LocalisationService_UseTrueTypeFont() && FontSupportsStringSprite(currencyDesc.symbol_unicode))
        {
            forceSpriteFont = true;
        }

        drawStringWithYOffsets(
            rt, buffer, { Drawing::Colour::black }, ps->screenPos, reinterpret_cast<int8_t*>(ps->yOffsets), forceSpriteFont,
            FontStyle::medium);
    } while ((ps = ps->nextEntry) != nullptr);
}
