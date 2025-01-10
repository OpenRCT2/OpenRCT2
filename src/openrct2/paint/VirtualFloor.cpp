/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "VirtualFloor.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../Input.h"
#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../profiling/Profiling.h"
#include "../sprites.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"
#include "Paint.h"
#include "VirtualFloor.h"
#include "tile_element/Paint.TileElement.h"

#include <limits>

using namespace OpenRCT2;

static constexpr uint16_t kVirtualFloorBaseSize = 5 * kCoordsXYStep;
static constexpr CoordsXY kVirtualFloorBaseSizeXY = { kVirtualFloorBaseSize, kVirtualFloorBaseSize };
static uint16_t _virtualFloorHeight = 0;
static CoordsXYZ _virtualFloorLastMinPos;
static CoordsXYZ _virtualFloorLastMaxPos;
static uint32_t _virtualFloorFlags = 0;

enum VirtualFloorFlags
{
    VIRTUAL_FLOOR_FLAG_NONE = 0,
    VIRTUAL_FLOOR_FLAG_ENABLED = (1 << 1),
    VIRTUAL_FLOOR_FORCE_INVALIDATION = (1 << 2),
};

bool VirtualFloorIsEnabled()
{
    return (_virtualFloorFlags & VIRTUAL_FLOOR_FLAG_ENABLED) != 0;
}

void VirtualFloorSetHeight(int16_t height)
{
    if (!VirtualFloorIsEnabled())
    {
        return;
    }

    if (_virtualFloorHeight != height)
    {
        VirtualFloorInvalidate();
        _virtualFloorHeight = height;
    }
}

static void VirtualFloorReset()
{
    _virtualFloorLastMinPos.x = std::numeric_limits<int32_t>::max();
    _virtualFloorLastMinPos.y = std::numeric_limits<int32_t>::max();
    _virtualFloorLastMaxPos.x = std::numeric_limits<int32_t>::lowest();
    _virtualFloorLastMaxPos.y = std::numeric_limits<int32_t>::lowest();
    _virtualFloorHeight = 0;
}

void VirtualFloorEnable()
{
    if (VirtualFloorIsEnabled())
    {
        return;
    }

    VirtualFloorReset();
    _virtualFloorFlags |= VIRTUAL_FLOOR_FLAG_ENABLED;
}

void VirtualFloorDisable()
{
    if (!VirtualFloorIsEnabled())
    {
        return;
    }

    _virtualFloorFlags &= ~VIRTUAL_FLOOR_FLAG_ENABLED;

    // Force invalidation, even if the position hasn't changed.
    _virtualFloorFlags |= VIRTUAL_FLOOR_FORCE_INVALIDATION;
    VirtualFloorInvalidate();
    _virtualFloorFlags &= ~VIRTUAL_FLOOR_FORCE_INVALIDATION;

    VirtualFloorReset();
}

void VirtualFloorInvalidate()
{
    PROFILED_FUNCTION();

    // First, let's figure out how big our selection is.
    CoordsXY min_position = { std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max() };
    CoordsXY max_position = { std::numeric_limits<int32_t>::lowest(), std::numeric_limits<int32_t>::lowest() };

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
    {
        min_position = gMapSelectPositionA;
        max_position = gMapSelectPositionB;
    }

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        for (const auto& tile : gMapSelectionTiles)
        {
            min_position.x = std::min(min_position.x, tile.x);
            min_position.y = std::min(min_position.y, tile.y);
            max_position.x = std::max(max_position.x, tile.x);
            max_position.y = std::max(max_position.y, tile.y);
        }
    }

    bool invalidateNewRegion
        = (min_position.x != std::numeric_limits<int32_t>::max() && min_position.y != std::numeric_limits<int32_t>::max()
           && max_position.x != std::numeric_limits<int32_t>::lowest()
           && max_position.y != std::numeric_limits<int32_t>::lowest());

    // Apply the virtual floor size to the computed invalidation area.
    min_position.x -= kVirtualFloorBaseSize + 16;
    min_position.y -= kVirtualFloorBaseSize + 16;
    max_position.x += kVirtualFloorBaseSize + 16;
    max_position.y += kVirtualFloorBaseSize + 16;

    // Invalidate previous region if appropriate.
    if (_virtualFloorLastMinPos.x != std::numeric_limits<int32_t>::max()
        && _virtualFloorLastMinPos.y != std::numeric_limits<int32_t>::max()
        && _virtualFloorLastMaxPos.x != std::numeric_limits<int32_t>::lowest()
        && _virtualFloorLastMaxPos.y != std::numeric_limits<int32_t>::lowest())
    {
        if (_virtualFloorLastMinPos != min_position || _virtualFloorLastMaxPos != max_position
            || (_virtualFloorFlags & VIRTUAL_FLOOR_FORCE_INVALIDATION) != 0)
        {
            LOG_VERBOSE(
                "Invalidating previous region, Min: %d %d, Max: %d %d", _virtualFloorLastMinPos.x, _virtualFloorLastMinPos.y,
                _virtualFloorLastMaxPos.x, _virtualFloorLastMaxPos.y);
            MapInvalidateRegion(_virtualFloorLastMinPos, _virtualFloorLastMaxPos);
        }
    }

    // Do not invalidate new region if floor hasn't moved.
    if (min_position == _virtualFloorLastMinPos && _virtualFloorLastMinPos.z == _virtualFloorHeight)
    {
        return;
    }

    if (!(_virtualFloorFlags & VIRTUAL_FLOOR_FLAG_ENABLED))
    {
        return;
    }

    LOG_VERBOSE("Min: %d %d, Max: %d %d", min_position.x, min_position.y, max_position.x, max_position.y);

    if (invalidateNewRegion)
    {
        MapInvalidateRegion(min_position, max_position);

        // Save minimal and maximal positions.
        _virtualFloorLastMinPos.x = min_position.x;
        _virtualFloorLastMinPos.y = min_position.y;
        _virtualFloorLastMinPos.z = _virtualFloorHeight;

        _virtualFloorLastMaxPos.x = max_position.x;
        _virtualFloorLastMaxPos.y = max_position.y;
        _virtualFloorLastMaxPos.z = _virtualFloorHeight;
    }
}

bool VirtualFloorTileIsFloor(const CoordsXY& loc)
{
    if (!VirtualFloorIsEnabled())
    {
        return false;
    }

    // Check if map selection (usually single tiles) are enabled
    // and if the current tile is near or on them
    // (short-circuit to false otherwise - we don't want to show a second
    //  virtual floor from e. g. an open ride construction window)
    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
    {
        return loc >= gMapSelectPositionA - kVirtualFloorBaseSizeXY && loc <= gMapSelectPositionB + kVirtualFloorBaseSizeXY;
    }

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        // Check if we are anywhere near the selection tiles (larger scenery / rides)
        for (const auto& tile : gMapSelectionTiles)
        {
            if (loc.x >= tile.x - kVirtualFloorBaseSize && loc.y >= tile.y - kVirtualFloorBaseSize
                && loc.x <= tile.x + kVirtualFloorBaseSize && loc.y <= tile.y + kVirtualFloorBaseSize)
            {
                return true;
            }
        }
    }

    return false;
}

static void VirtualFloorGetTileProperties(
    const CoordsXY& loc, int16_t height, bool* outOccupied, bool* tileOwned, uint8_t* outOccupiedEdges, bool* outBelowGround,
    bool* aboveGround, bool* outLit)
{
    *outOccupied = false;
    *outOccupiedEdges = 0;
    *outBelowGround = false;
    *outLit = false;
    *aboveGround = false;
    *tileOwned = false;

    // See if we are a selected tile
    if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
    {
        if (loc >= gMapSelectPositionA && loc <= gMapSelectPositionB)
        {
            *outLit = true;
        }
    }

    // See if we are on top of the selection tiles
    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        for (const auto& tile : gMapSelectionTiles)
        {
            if (tile == loc)
            {
                *outLit = true;
                break;
            }
        }
    }

    *tileOwned = MapIsLocationOwned({ loc, height });

    if (GetGameState().Cheats.sandboxMode)
        *tileOwned = true;

    // Iterate through the map elements of the current tile to find:
    //  * Surfaces, which may put us underground
    //  * Walls / banners, which are displayed as occupied edges
    //  * Ghost objects, which are displayed as lit squares
    for (auto* tileElement : TileElementsView(loc))
    {
        const auto elementType = tileElement->GetType();

        if (elementType == TileElementType::Surface)
        {
            if (height < tileElement->GetClearanceZ())
            {
                *outBelowGround = true;
            }
            else if (height < (tileElement->GetBaseZ() + LAND_HEIGHT_STEP) && tileElement->AsSurface()->GetSlope() != 0)
            {
                *outBelowGround = true;
                *outOccupied = true;
            }
            if (height > tileElement->GetBaseZ())
            {
                *aboveGround = true;
            }
            continue;
        }

        if (height >= tileElement->GetClearanceZ() || height < tileElement->GetBaseZ())
        {
            continue;
        }

        if (elementType == TileElementType::Wall || elementType == TileElementType::Banner)
        {
            int32_t direction = tileElement->GetDirection();
            *outOccupiedEdges |= 1 << direction;
            continue;
        }

        if (tileElement->IsGhost())
        {
            *outLit = true;
            continue;
        }

        *outOccupied = true;
    }
}

void VirtualFloorPaint(PaintSession& session)
{
    PROFILED_FUNCTION();

    static constexpr CoordsXY scenery_half_tile_offsets[4] = {
        { -kCoordsXYStep, 0 },
        { 0, kCoordsXYStep },
        { kCoordsXYStep, 0 },
        { 0, -kCoordsXYStep },
    };

    if (_virtualFloorHeight < kMinimumLandHeight)
        return;

    uint8_t direction = session.CurrentRotation;

    // This is a virtual floor, so no interactions
    session.InteractionType = ViewportInteractionItem::None;

    int16_t virtualFloorClipHeight = _virtualFloorHeight;

    // Check for occupation and walls
    bool weAreOccupied;
    uint8_t occupiedEdges;
    bool weAreBelowGround;
    bool weAreLit;
    bool weAreOwned;
    bool weAreAboveGround;
    uint8_t litEdges = 0;

    VirtualFloorGetTileProperties(
        session.MapPosition, virtualFloorClipHeight, &weAreOccupied, &weAreOwned, &occupiedEdges, &weAreBelowGround,
        &weAreAboveGround, &weAreLit);

    // Move the bits around to match the current rotation
    occupiedEdges |= occupiedEdges << 4;
    occupiedEdges >>= (4 - direction);
    occupiedEdges &= 0x0F;

    // Try the four tiles next to us for the same parameters as above,
    //  if our parameters differ we set an edge towards that tile
    for (uint8_t i = 0; i < kNumOrthogonalDirections; i++)
    {
        uint8_t effectiveRotation = (kNumOrthogonalDirections + i - direction) % kNumOrthogonalDirections;
        CoordsXY theirLocation = session.MapPosition + scenery_half_tile_offsets[effectiveRotation];

        bool theyAreOccupied;
        uint8_t theirOccupiedEdges;
        bool theyAreBelowGround;
        bool theyAreLit;
        bool theyAreOwned;
        bool theyAreAboveGround;

        VirtualFloorGetTileProperties(
            theirLocation, virtualFloorClipHeight, &theyAreOccupied, &theyAreOwned, &theirOccupiedEdges, &theyAreBelowGround,
            &theyAreAboveGround, &theyAreLit);

        if (theirOccupiedEdges & (1 << ((effectiveRotation + 2) % kNumOrthogonalDirections)) && (weAreOwned && !theyAreOwned))
        {
            occupiedEdges |= 1 << i;
        }
        if (weAreLit != theyAreLit || (weAreOwned && !theyAreOwned))
        {
            litEdges |= 1 << i;
        }
        else if ((weAreOccupied != theyAreOccupied || weAreBelowGround != theyAreBelowGround) && weAreOwned)
        {
            occupiedEdges |= 1 << i;
        }
    }

    const ImageId remap_base = ImageId(0, COLOUR_DARK_PURPLE);
    const ImageId remap_edge = ImageId(0, COLOUR_WHITE);
    const ImageId remap_lit = ImageId(0, COLOUR_DARK_BROWN);

    // Edges which are internal to objects (i.e., the tile on both sides
    //  is occupied/lit) are not rendered to provide visual clarity.
    uint8_t dullEdges = 0xF & ~occupiedEdges & ~litEdges;
    uint8_t paintEdges = ((weAreOccupied || weAreLit) && weAreOwned) ? ~dullEdges : 0xF;

    const auto virtualFloorOffset = CoordsXYZ{ 0, 0, _virtualFloorHeight };
    if (paintEdges & EDGE_NE)
    {
        const auto baseImg = !(occupiedEdges & EDGE_NE) ? ((litEdges & EDGE_NE) ? remap_lit : remap_base) : remap_edge;
        PaintAddImageAsParent(
            session, baseImg.WithIndex(SPR_G2_SELECTION_EDGE_NE), virtualFloorOffset,
            { { 5, 5, _virtualFloorHeight + ((dullEdges & EDGE_NE) ? -2 : 0) }, { 0, 0, 1 } });
    }
    if (paintEdges & EDGE_SE)
    {
        const auto baseImg = !(occupiedEdges & EDGE_SE) ? ((litEdges & EDGE_SE) ? remap_lit : remap_base) : remap_edge;
        PaintAddImageAsParent(
            session, baseImg.WithIndex(SPR_G2_SELECTION_EDGE_SE), virtualFloorOffset,
            { { 16, 27, _virtualFloorHeight + ((dullEdges & EDGE_SE) ? -2 : 0) }, { 1, 1, 1 } });
    }
    if (paintEdges & EDGE_SW)
    {
        const auto baseImg = !(occupiedEdges & EDGE_SW) ? ((litEdges & EDGE_SW) ? remap_lit : remap_base) : remap_edge;
        PaintAddImageAsParent(
            session, baseImg.WithIndex(SPR_G2_SELECTION_EDGE_SW), virtualFloorOffset,
            { { 27, 16, _virtualFloorHeight + ((dullEdges & EDGE_SW) ? -2 : 0) }, { 1, 1, 1 } });
    }
    if (paintEdges & EDGE_NW)
    {
        const auto baseImg = !(occupiedEdges & EDGE_NW) ? ((litEdges & EDGE_NW) ? remap_lit : remap_base) : remap_edge;
        PaintAddImageAsParent(
            session, baseImg.WithIndex(SPR_G2_SELECTION_EDGE_NW), virtualFloorOffset,
            { { 5, 5, _virtualFloorHeight + ((dullEdges & EDGE_NW) ? -2 : 0) }, { 0, 0, 1 } });
    }

    if (Config::Get().general.VirtualFloorStyle != VirtualFloorStyles::Glassy)
        return;

    if (!weAreOccupied && !weAreLit && weAreAboveGround && weAreOwned)
    {
        auto imageColourFlats = ImageId(SPR_G2_SURFACE_GLASSY_RECOLOURABLE, FilterPaletteID::PaletteWater).WithBlended(true);
        PaintAddImageAsParent(
            session, imageColourFlats, virtualFloorOffset, { { 2, 2, _virtualFloorHeight - 3 }, { 30, 30, 0 } });
    }
}

uint16_t VirtualFloorGetHeight()
{
    return _virtualFloorHeight;
}
