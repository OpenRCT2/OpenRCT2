/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.PathAddition.h"

#include "../../core/Numerics.hpp"
#include "../../drawing/LightFX.h"
#include "../../interface/Viewport.h"
#include "../../object/PathAdditionEntry.h"
#include "../../profiling/Profiling.h"
#include "../../world/tile_element/PathElement.h"
#include "Paint.TileElement.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

using OpenRCT2::Drawing::LightFx::LightType;

static ImageIndex GetEdgeImageOffset(edge_t edge)
{
    switch (edge)
    {
        case edgeNE:
            return 1;
        case edgeSE:
            return 2;
        case edgeSW:
            return 3;
        case edgeNW:
            return 4;
        default:
            return 0;
    }
}

static ImageIndex GetFootpathLampImage(const PathAdditionEntry& pathAdditionEntry, edge_t edge, bool isBroken)
{
    auto offset = GetEdgeImageOffset(edge);
    if (offset == 0)
        return kImageIndexUndefined;
    return pathAdditionEntry.image + offset + (isBroken ? 4 : 0);
}

static ImageIndex GetFootpathBinImage(const PathAdditionEntry& pathAdditionEntry, edge_t edge, bool isBroken, bool isFull)
{
    auto offset = GetEdgeImageOffset(edge);
    if (offset == 0)
        return kImageIndexUndefined;

    auto stateOffset = isBroken ? 4 : (isFull ? 8 : 0);
    return pathAdditionEntry.image + offset + stateOffset;
}

static ImageIndex GetFootpathBenchImage(const PathAdditionEntry& pathAdditionEntry, edge_t edge, bool isBroken)
{
    auto offset = GetEdgeImageOffset(edge);
    if (offset == 0)
        return kImageIndexUndefined;
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
    if (edges & edgeNE)
    {
        auto imageIndex = GetFootpathLampImage(pathAdditionEntry, edgeNE, isBroken);
        paintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 2, 16, height }, { { 3, 8, height + 2 }, { 0, 16, 23 } });
    }
    if (edges & edgeSE)
    {
        auto imageIndex = GetFootpathLampImage(pathAdditionEntry, edgeSE, isBroken);
        paintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 16, 30, height }, { { 2, 29, height + 2 }, { 22, 0, 23 } });
    }
    if (edges & edgeSW)
    {
        auto imageIndex = GetFootpathLampImage(pathAdditionEntry, edgeSW, isBroken);
        paintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 30, 16, height }, { { 29, 2, height + 2 }, { 0, 22, 23 } });
    }
    if (edges & edgeNW)
    {
        auto imageIndex = GetFootpathLampImage(pathAdditionEntry, edgeNW, isBroken);
        paintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 16, 2, height }, { { 8, 3, height + 2 }, { 16, 0, 23 } });
    }
}

static bool IsBinFull(PaintSession& session, const PathElement& pathElement, edge_t edge)
{
    switch (edge)
    {
        case edgeNE:
            return !(pathElement.GetAdditionStatus() & Numerics::ror8(0x03, (2 * session.currentRotation)));
        case edgeSE:
            return !(pathElement.GetAdditionStatus() & Numerics::ror8(0x0C, (2 * session.currentRotation)));
        case edgeSW:
            return !(pathElement.GetAdditionStatus() & Numerics::ror8(0x30, (2 * session.currentRotation)));
        case edgeNW:
            return !(pathElement.GetAdditionStatus() & Numerics::ror8(0xC0, (2 * session.currentRotation)));
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
    auto highlightPathIssues = (session.viewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) != 0;

    if (edges & edgeNE)
    {
        auto binIsFull = IsBinFull(session, pathElement, edgeNE);
        auto imageIndex = GetFootpathBinImage(pathAdditionEntry, edgeNE, binsAreVandalised, binIsFull);
        if (!highlightPathIssues || binIsFull || binsAreVandalised)
            paintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { 7, 16, height }, { { 6, 8, height + 2 }, { 0, 16, 7 } });
    }
    if (edges & edgeSE)
    {
        auto binIsFull = IsBinFull(session, pathElement, edgeSE);
        auto imageIndex = GetFootpathBinImage(pathAdditionEntry, edgeSE, binsAreVandalised, binIsFull);
        if (!highlightPathIssues || binIsFull || binsAreVandalised)
            paintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { 16, 25, height }, { { 8, 23, height + 2 }, { 16, 0, 7 } });
    }
    if (edges & edgeSW)
    {
        auto binIsFull = IsBinFull(session, pathElement, edgeSW);
        auto imageIndex = GetFootpathBinImage(pathAdditionEntry, edgeSW, binsAreVandalised, binIsFull);
        if (!highlightPathIssues || binIsFull || binsAreVandalised)
            paintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { 25, 16, height }, { { 23, 8, height + 2 }, { 0, 16, 7 } });
    }
    if (edges & edgeNW)
    {
        auto binIsFull = IsBinFull(session, pathElement, edgeNW);
        auto imageIndex = GetFootpathBinImage(pathAdditionEntry, edgeNW, binsAreVandalised, binIsFull);
        if (!highlightPathIssues || binIsFull || binsAreVandalised)
            paintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { 16, 7, height }, { { 8, 6, height + 2 }, { 16, 0, 7 } });
    }
}

/* rct2: 0x006A5E81 */
static void PathAdditionBenchesPaint(
    PaintSession& session, const PathAdditionEntry& pathAdditionEntry, const PathElement& pathElement, int32_t height,
    uint8_t edges, ImageId imageTemplate)
{
    auto isBroken = pathElement.IsBroken();
    if (edges & edgeNE)
    {
        auto imageIndex = GetFootpathBenchImage(pathAdditionEntry, edgeNE, isBroken);
        paintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 7, 16, height }, { { 6, 8, height + 2 }, { 0, 16, 7 } });
    }
    if (edges & edgeSE)
    {
        auto imageIndex = GetFootpathBenchImage(pathAdditionEntry, edgeSE, isBroken);
        paintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 16, 25, height }, { { 8, 23, height + 2 }, { 16, 0, 7 } });
    }

    if (edges & edgeSW)
    {
        auto imageIndex = GetFootpathBenchImage(pathAdditionEntry, edgeSW, isBroken);
        paintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 25, 16, height }, { { 23, 8, height + 2 }, { 0, 16, 7 } });
    }

    if (edges & edgeNW)
    {
        auto imageIndex = GetFootpathBenchImage(pathAdditionEntry, edgeNW, isBroken);
        paintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { 16, 7, height }, { { 8, 6, height + 2 }, { 16, 0, 7 } });
    }
}

/* rct2: 0x006A6008 */
static void PathAdditionJumpingFountainsPaint(
    PaintSession& session, const PathAdditionEntry& pathAdditionEntry, int32_t height, ImageId imageTemplate, RenderTarget& rt)
{
    if (rt.zoom_level > ZoomLevel{ 0 })
        return;

    auto imageId = imageTemplate.WithIndex(pathAdditionEntry.image);
    paintAddImageAsParent(session, imageId.WithIndexOffset(1), { 0, 0, height }, { { 6, 8, height + 2 }, { 1, 1, 2 } });
    paintAddImageAsParent(session, imageId.WithIndexOffset(2), { 0, 0, height }, { { 8, 23, height + 2 }, { 1, 1, 2 } });
    paintAddImageAsParent(session, imageId.WithIndexOffset(3), { 0, 0, height }, { { 23, 8, height + 2 }, { 1, 1, 2 } });
    paintAddImageAsParent(session, imageId.WithIndexOffset(4), { 0, 0, height }, { { 8, 6, height + 2 }, { 1, 1, 2 } });
}

inline bool PathAdditionIsVisible(uint32_t viewFlags, const PathAdditionEntry& pathAdditionEntry, bool isBroken)
{
    if (!(viewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        return true;

    if (isBroken)
        return true;

    if (pathAdditionEntry.draw_type == PathAdditionDrawType::bin)
        return true;

    return false;
}

void paintPathAddition(PaintSession& session, const PathElement& pathElement, uint16_t height, ImageId sceneryImageTemplate)
{
    // Path additions get drawn on edges that are not connected, so we need to flip them.
    const auto edges = pathElement.GetEdges() ^ 0b1111;
    const auto rotatedEdges = ((edges << session.currentRotation) & 0xF) | (((edges) << session.currentRotation) >> 4);

    session.interactionType = ViewportInteractionItem::pathAddition;
    if (sceneryImageTemplate.IsRemap())
    {
        session.interactionType = ViewportInteractionItem::none;
    }

    auto* pathAddEntry = pathElement.GetAdditionEntry();
    // Can be null if the object is not loaded.
    if (pathAddEntry == nullptr || !PathAdditionIsVisible(session.viewFlags, *pathAddEntry, pathElement.IsBroken()))
    {
        return;
    }

    switch (pathAddEntry->draw_type)
    {
        case PathAdditionDrawType::light:
            PathAdditionLightsPaint(session, *pathAddEntry, pathElement, height, rotatedEdges, sceneryImageTemplate);
            break;
        case PathAdditionDrawType::bin:
            PathAdditionBinsPaint(session, *pathAddEntry, pathElement, height, rotatedEdges, sceneryImageTemplate);
            break;
        case PathAdditionDrawType::bench:
            PathAdditionBenchesPaint(session, *pathAddEntry, pathElement, height, rotatedEdges, sceneryImageTemplate);
            break;
        case PathAdditionDrawType::jumpingFountain:
            PathAdditionJumpingFountainsPaint(session, *pathAddEntry, height, sceneryImageTemplate, session.rt);
            break;
    }

    session.interactionType = ViewportInteractionItem::footpath;

    // Set when the path addition is a ghost.
    if (sceneryImageTemplate.IsRemap())
    {
        session.interactionType = ViewportInteractionItem::none;
    }
}

void PaintLampLightEffects(PaintSession& session, const PathElement& pathEl, uint16_t height)
{
    PROFILED_FUNCTION();

    if (LightFx::IsAvailable())
    {
        if (pathEl.HasAddition() && !(pathEl.IsBroken()))
        {
            auto* pathAddEntry = pathEl.GetAdditionEntry();
            if (pathAddEntry != nullptr && pathAddEntry->flags & PATH_ADDITION_FLAG_LAMP)
            {
                if (!(pathEl.GetEdges() & edgeNE))
                {
                    LightFx::Add3DLightMagicFromDrawingTile(session.mapPosition, -16, 0, height + 23, LightType::Lantern3);
                }
                if (!(pathEl.GetEdges() & edgeSE))
                {
                    LightFx::Add3DLightMagicFromDrawingTile(session.mapPosition, 0, 16, height + 23, LightType::Lantern3);
                }
                if (!(pathEl.GetEdges() & edgeSW))
                {
                    LightFx::Add3DLightMagicFromDrawingTile(session.mapPosition, 16, 0, height + 23, LightType::Lantern3);
                }
                if (!(pathEl.GetEdges() & edgeNW))
                {
                    LightFx::Add3DLightMagicFromDrawingTile(session.mapPosition, 0, -16, height + 23, LightType::Lantern3);
                }
            }
        }
    }
}
