/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.PathAddition.h"

#include "../../core/Numerics.hpp"
#include "../../drawing/LightFX.h"
#include "../../object/PathAdditionEntry.h"
#include "../../profiling/Profiling.h"
#include "Paint.TileElement.h"

static ImageIndex GetEdgeImageOffset(edge_t edge)
{
    switch (edge)
    {
        case EDGE_NE:
            return 1;
        case EDGE_SE:
            return 2;
        case EDGE_SW:
            return 3;
        case EDGE_NW:
            return 4;
        default:
            return 0;
    }
}

static ImageIndex GetFootpathLampImage(const PathAdditionEntry& pathAdditionEntry, edge_t edge, bool isBroken)
{
    auto offset = GetEdgeImageOffset(edge);
    if (offset == 0)
        return ImageIndexUndefined;
    return pathAdditionEntry.image + offset + (isBroken ? 4 : 0);
}

static ImageIndex GetFootpathBinImage(const PathAdditionEntry& pathAdditionEntry, edge_t edge, bool isBroken, bool isFull)
{
    auto offset = GetEdgeImageOffset(edge);
    if (offset == 0)
        return ImageIndexUndefined;

    auto stateOffset = isBroken ? 4 : (isFull ? 8 : 0);
    return pathAdditionEntry.image + offset + stateOffset;
}

static ImageIndex GetFootpathBenchImage(const PathAdditionEntry& pathAdditionEntry, edge_t edge, bool isBroken)
{
    auto offset = GetEdgeImageOffset(edge);
    if (offset == 0)
        return ImageIndexUndefined;
    return pathAdditionEntry.image + offset + (isBroken ? 4 : 0);
}

/* rct2: 0x006A5AE5 */
static void PathAdditionLightsPaint(
    PaintSession& session, const PathAdditionEntry& pathAdditionEntry, const PathElement& pathElement, int32_t height,
    uint8_t edges, ImageId imageTemplate)
{
    if (pathElement.IsSloped())
        height += 8;

    auto isBroken = pathElement.IsBroken();
    if (edges & EDGE_NE)
    {
        auto imageIndex = GetFootpathLampImage(pathAdditionEntry, EDGE_NE, isBroken);
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 2, 16, height }, { { 6, 8, height + 2 }, { 0, 16, 23 } });
    }
    if (edges & EDGE_SE)
    {
        auto imageIndex = GetFootpathLampImage(pathAdditionEntry, EDGE_SE, isBroken);
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 16, 30, height }, { { 8, 23, height + 2 }, { 16, 0, 23 } });
    }
    if (edges & EDGE_SW)
    {
        auto imageIndex = GetFootpathLampImage(pathAdditionEntry, EDGE_SW, isBroken);
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 30, 16, height }, { { 23, 8, height + 2 }, { 0, 16, 23 } });
    }
    if (edges & EDGE_NW)
    {
        auto imageIndex = GetFootpathLampImage(pathAdditionEntry, EDGE_NW, isBroken);
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 16, 2, height }, { { 8, 6, height + 2 }, { 16, 0, 23 } });
    }
}

static bool IsBinFull(PaintSession& session, const PathElement& pathElement, edge_t edge)
{
    switch (edge)
    {
        case EDGE_NE:
            return !(pathElement.GetAdditionStatus() & Numerics::ror8(0x03, (2 * session.CurrentRotation)));
        case EDGE_SE:
            return !(pathElement.GetAdditionStatus() & Numerics::ror8(0x0C, (2 * session.CurrentRotation)));
        case EDGE_SW:
            return !(pathElement.GetAdditionStatus() & Numerics::ror8(0x30, (2 * session.CurrentRotation)));
        case EDGE_NW:
            return !(pathElement.GetAdditionStatus() & Numerics::ror8(0xC0, (2 * session.CurrentRotation)));
        default:
            return false;
    }
}

/* rct2: 0x006A5C94 */
static void PathAdditionBinsPaint(
    PaintSession& session, const PathAdditionEntry& pathAdditionEntry, const PathElement& pathElement, int32_t height,
    uint8_t edges, ImageId imageTemplate)
{
    if (pathElement.IsSloped())
        height += 8;

    bool binsAreVandalised = pathElement.IsBroken();
    auto highlightPathIssues = (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) != 0;

    if (edges & EDGE_NE)
    {
        auto binIsFull = IsBinFull(session, pathElement, EDGE_NE);
        auto imageIndex = GetFootpathBinImage(pathAdditionEntry, EDGE_NE, binsAreVandalised, binIsFull);
        if (!highlightPathIssues || binIsFull || binsAreVandalised)
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { 7, 16, height }, { { 6, 8, height + 2 }, { 0, 16, 7 } });
    }
    if (edges & EDGE_SE)
    {
        auto binIsFull = IsBinFull(session, pathElement, EDGE_SE);
        auto imageIndex = GetFootpathBinImage(pathAdditionEntry, EDGE_SE, binsAreVandalised, binIsFull);
        if (!highlightPathIssues || binIsFull || binsAreVandalised)
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { 16, 25, height }, { { 8, 23, height + 2 }, { 16, 0, 7 } });
    }
    if (edges & EDGE_SW)
    {
        auto binIsFull = IsBinFull(session, pathElement, EDGE_SW);
        auto imageIndex = GetFootpathBinImage(pathAdditionEntry, EDGE_SW, binsAreVandalised, binIsFull);
        if (!highlightPathIssues || binIsFull || binsAreVandalised)
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { 25, 16, height }, { { 23, 8, height + 2 }, { 0, 16, 7 } });
    }
    if (edges & EDGE_NW)
    {
        auto binIsFull = IsBinFull(session, pathElement, EDGE_NW);
        auto imageIndex = GetFootpathBinImage(pathAdditionEntry, EDGE_NW, binsAreVandalised, binIsFull);
        if (!highlightPathIssues || binIsFull || binsAreVandalised)
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { 16, 7, height }, { { 8, 6, height + 2 }, { 16, 0, 7 } });
    }
}

/* rct2: 0x006A5E81 */
static void PathAdditionBenchesPaint(
    PaintSession& session, const PathAdditionEntry& pathAdditionEntry, const PathElement& pathElement, int32_t height,
    uint8_t edges, ImageId imageTemplate)
{
    auto isBroken = pathElement.IsBroken();
    if (edges & EDGE_NE)
    {
        auto imageIndex = GetFootpathBenchImage(pathAdditionEntry, EDGE_NE, isBroken);
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 7, 16, height }, { { 6, 8, height + 2 }, { 0, 16, 7 } });
    }
    if (edges & EDGE_SE)
    {
        auto imageIndex = GetFootpathBenchImage(pathAdditionEntry, EDGE_SE, isBroken);
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 16, 25, height }, { { 8, 23, height + 2 }, { 16, 0, 7 } });
    }

    if (edges & EDGE_SW)
    {
        auto imageIndex = GetFootpathBenchImage(pathAdditionEntry, EDGE_SW, isBroken);
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 25, 16, height }, { { 23, 8, height + 2 }, { 0, 16, 7 } });
    }

    if (edges & EDGE_NW)
    {
        auto imageIndex = GetFootpathBenchImage(pathAdditionEntry, EDGE_NW, isBroken);
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 16, 7, height }, { { 8, 6, height + 2 }, { 16, 0, 7 } });
    }
}

/* rct2: 0x006A6008 */
static void PathAdditionJumpingFountainsPaint(
    PaintSession& session, const PathAdditionEntry& pathAdditionEntry, int32_t height, ImageId imageTemplate,
    DrawPixelInfo& dpi)
{
    if (dpi.zoom_level > ZoomLevel{ 0 })
        return;

    auto imageId = imageTemplate.WithIndex(pathAdditionEntry.image);
    PaintAddImageAsParent(session, imageId.WithIndexOffset(1), { 0, 0, height }, { { 6, 8, height + 2 }, { 1, 1, 2 } });
    PaintAddImageAsParent(session, imageId.WithIndexOffset(2), { 0, 0, height }, { { 8, 23, height + 2 }, { 1, 1, 2 } });
    PaintAddImageAsParent(session, imageId.WithIndexOffset(3), { 0, 0, height }, { { 23, 8, height + 2 }, { 1, 1, 2 } });
    PaintAddImageAsParent(session, imageId.WithIndexOffset(4), { 0, 0, height }, { { 8, 6, height + 2 }, { 1, 1, 2 } });
}

inline bool PathAdditionIsVisible(uint32_t viewFlags, const PathAdditionEntry& pathAdditionEntry, bool isBroken)
{
    if (!(viewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        return true;

    if (isBroken)
        return true;

    if (pathAdditionEntry.draw_type == PathAdditionDrawType::Bin)
        return true;

    return false;
}

void Sub6A3F61PathAddition(PaintSession& session, const PathElement& pathElement, uint16_t height, ImageId sceneryImageTemplate)
{
    // Path additions get drawn on edges that are not connected, so we need to flip them.
    const auto edges = pathElement.GetEdges() ^ 0b1111;
    const auto rotatedEdges = ((edges << session.CurrentRotation) & 0xF) | (((edges) << session.CurrentRotation) >> 4);

    session.InteractionType = ViewportInteractionItem::PathAddition;
    if (sceneryImageTemplate.IsRemap())
    {
        session.InteractionType = ViewportInteractionItem::None;
    }

    auto* pathAddEntry = pathElement.GetAdditionEntry();
    // Can be null if the object is not loaded.
    if (pathAddEntry == nullptr || !PathAdditionIsVisible(session.ViewFlags, *pathAddEntry, pathElement.IsBroken()))
    {
        return;
    }

    switch (pathAddEntry->draw_type)
    {
        case PathAdditionDrawType::Light:
            PathAdditionLightsPaint(session, *pathAddEntry, pathElement, height, rotatedEdges, sceneryImageTemplate);
            break;
        case PathAdditionDrawType::Bin:
            PathAdditionBinsPaint(session, *pathAddEntry, pathElement, height, rotatedEdges, sceneryImageTemplate);
            break;
        case PathAdditionDrawType::Bench:
            PathAdditionBenchesPaint(session, *pathAddEntry, pathElement, height, rotatedEdges, sceneryImageTemplate);
            break;
        case PathAdditionDrawType::JumpingFountain:
            PathAdditionJumpingFountainsPaint(session, *pathAddEntry, height, sceneryImageTemplate, session.DPI);
            break;
    }

    session.InteractionType = ViewportInteractionItem::Footpath;

    // Set when the path addition is a ghost.
    if (sceneryImageTemplate.IsRemap())
    {
        session.InteractionType = ViewportInteractionItem::None;
    }
}

void PaintLampLightEffects(PaintSession& session, const PathElement& pathEl, uint16_t height)
{
    PROFILED_FUNCTION();

    if (LightFXIsAvailable())
    {
        if (pathEl.HasAddition() && !(pathEl.IsBroken()))
        {
            auto* pathAddEntry = pathEl.GetAdditionEntry();
            if (pathAddEntry != nullptr && pathAddEntry->flags & PATH_ADDITION_FLAG_LAMP)
            {
                if (!(pathEl.GetEdges() & EDGE_NE))
                {
                    LightFXAdd3DLightMagicFromDrawingTile(session.MapPosition, -16, 0, height + 23, LightType::Lantern3);
                }
                if (!(pathEl.GetEdges() & EDGE_SE))
                {
                    LightFXAdd3DLightMagicFromDrawingTile(session.MapPosition, 0, 16, height + 23, LightType::Lantern3);
                }
                if (!(pathEl.GetEdges() & EDGE_SW))
                {
                    LightFXAdd3DLightMagicFromDrawingTile(session.MapPosition, 16, 0, height + 23, LightType::Lantern3);
                }
                if (!(pathEl.GetEdges() & EDGE_NW))
                {
                    LightFXAdd3DLightMagicFromDrawingTile(session.MapPosition, 0, -16, height + 23, LightType::Lantern3);
                }
            }
        }
    }
}
