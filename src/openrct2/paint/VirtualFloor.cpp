/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "VirtualFloor.h"

#include "../Cheats.h"
#include "../Input.h"
#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../profiling/Profiling.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/TileElementsView.h"
#include "Paint.h"
#include "VirtualFloor.h"
#include "tile_element/Paint.TileElement.h"

#include <algorithm>
#include <limits>

using namespace OpenRCT2;

static uint16_t _virtualFloorBaseSize = 5 * 32;
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

bool virtual_floor_is_enabled()
{
    return (_virtualFloorFlags & VIRTUAL_FLOOR_FLAG_ENABLED) != 0;
}

void virtual_floor_set_height(int16_t height)
{
    if (!virtual_floor_is_enabled())
    {
        return;
    }

    if (_virtualFloorHeight != height)
    {
        virtual_floor_invalidate();
        _virtualFloorHeight = height;
    }
}

static void virtual_floor_reset()
{
    _virtualFloorLastMinPos.x = std::numeric_limits<int32_t>::max();
    _virtualFloorLastMinPos.y = std::numeric_limits<int32_t>::max();
    _virtualFloorLastMaxPos.x = std::numeric_limits<int32_t>::lowest();
    _virtualFloorLastMaxPos.y = std::numeric_limits<int32_t>::lowest();
    _virtualFloorHeight = 0;
}

void virtual_floor_enable()
{
    if (virtual_floor_is_enabled())
    {
        return;
    }

    virtual_floor_reset();
    _virtualFloorFlags |= VIRTUAL_FLOOR_FLAG_ENABLED;
}

void virtual_floor_disable()
{
    if (!virtual_floor_is_enabled())
    {
        return;
    }

    _virtualFloorFlags &= ~VIRTUAL_FLOOR_FLAG_ENABLED;

    // Force invalidation, even if the position hasn't changed.
    _virtualFloorFlags |= VIRTUAL_FLOOR_FORCE_INVALIDATION;
    virtual_floor_invalidate();
    _virtualFloorFlags &= ~VIRTUAL_FLOOR_FORCE_INVALIDATION;

    virtual_floor_reset();
}

void virtual_floor_invalidate()
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

    // Apply the virtual floor size to the computed invalidation area.
    min_position.x -= _virtualFloorBaseSize + 16;
    min_position.y -= _virtualFloorBaseSize + 16;
    max_position.x += _virtualFloorBaseSize + 16;
    max_position.y += _virtualFloorBaseSize + 16;

    // Invalidate previous region if appropriate.
    if (_virtualFloorLastMinPos.x != std::numeric_limits<int32_t>::max()
        && _virtualFloorLastMinPos.y != std::numeric_limits<int32_t>::max()
        && _virtualFloorLastMaxPos.x != std::numeric_limits<int32_t>::lowest()
        && _virtualFloorLastMaxPos.y != std::numeric_limits<int32_t>::lowest())
    {
        if (_virtualFloorLastMinPos != min_position || _virtualFloorLastMaxPos != max_position
            || (_virtualFloorFlags & VIRTUAL_FLOOR_FORCE_INVALIDATION) != 0)
        {
            log_verbose(
                "Invalidating previous region, Min: %d %d, Max: %d %d", _virtualFloorLastMinPos.x, _virtualFloorLastMinPos.y,
                _virtualFloorLastMaxPos.x, _virtualFloorLastMaxPos.y);
            map_invalidate_region(_virtualFloorLastMinPos, _virtualFloorLastMaxPos);
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

    log_verbose("Min: %d %d, Max: %d %d", min_position.x, min_position.y, max_position.x, max_position.y);

    // Invalidate new region if coordinates are set.
    if (min_position.x != std::numeric_limits<int32_t>::max() && min_position.y != std::numeric_limits<int32_t>::max()
        && max_position.x != std::numeric_limits<int32_t>::lowest() && max_position.y != std::numeric_limits<int32_t>::lowest())
    {
        map_invalidate_region(min_position, max_position);

        // Save minimal and maximal positions.
        _virtualFloorLastMinPos.x = min_position.x;
        _virtualFloorLastMinPos.y = min_position.y;
        _virtualFloorLastMinPos.z = _virtualFloorHeight;

        _virtualFloorLastMaxPos.x = max_position.x;
        _virtualFloorLastMaxPos.y = max_position.y;
        _virtualFloorLastMaxPos.z = _virtualFloorHeight;
    }
}

bool virtual_floor_tile_is_floor(const CoordsXY& loc)
{
    if (!virtual_floor_is_enabled())
    {
        return false;
    }

    // Check if map selection (usually single tiles) are enabled
    //  and if the current tile is near or on them
    if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE) && loc.x >= gMapSelectPositionA.x - _virtualFloorBaseSize
        && loc.y >= gMapSelectPositionA.y - _virtualFloorBaseSize && loc.x <= gMapSelectPositionB.x + _virtualFloorBaseSize
        && loc.y <= gMapSelectPositionB.y + _virtualFloorBaseSize)
    {
        return true;
    }

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        // Check if we are anywhere near the selection tiles (larger scenery / rides)
        for (const auto& tile : gMapSelectionTiles)
        {
            if (loc.x >= tile.x - _virtualFloorBaseSize && loc.y >= tile.y - _virtualFloorBaseSize
                && loc.x <= tile.x + _virtualFloorBaseSize && loc.y <= tile.y + _virtualFloorBaseSize)
            {
                return true;
            }
        }
    }

    return false;
}

static void virtual_floor_get_tile_properties(
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

    *tileOwned = map_is_location_owned({ loc, height });

    if (gCheatsSandboxMode)
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

void virtual_floor_paint(paint_session& session)
{
    PROFILED_FUNCTION();

    static constexpr const CoordsXY scenery_half_tile_offsets[4] = {
        { -COORDS_XY_STEP, 0 },
        { 0, COORDS_XY_STEP },
        { COORDS_XY_STEP, 0 },
        { 0, -COORDS_XY_STEP },
    };

    if (_virtualFloorHeight < MINIMUM_LAND_HEIGHT)
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

    virtual_floor_get_tile_properties(
        session.MapPosition, virtualFloorClipHeight, &weAreOccupied, &weAreOwned, &occupiedEdges, &weAreBelowGround,
        &weAreAboveGround, &weAreLit);

    // Move the bits around to match the current rotation
    occupiedEdges |= occupiedEdges << 4;
    occupiedEdges >>= (4 - direction);
    occupiedEdges &= 0x0F;

    // Try the four tiles next to us for the same parameters as above,
    //  if our parameters differ we set an edge towards that tile
    for (uint8_t i = 0; i < NumOrthogonalDirections; i++)
    {
        uint8_t effectiveRotation = (NumOrthogonalDirections + i - direction) % NumOrthogonalDirections;
        CoordsXY theirLocation = session.MapPosition + scenery_half_tile_offsets[effectiveRotation];

        bool theyAreOccupied;
        uint8_t theirOccupiedEdges;
        bool theyAreBelowGround;
        bool theyAreLit;
        bool theyAreOwned;
        bool theyAreAboveGround;

        virtual_floor_get_tile_properties(
            theirLocation, virtualFloorClipHeight, &theyAreOccupied, &theyAreOwned, &theirOccupiedEdges, &theyAreBelowGround,
            &theyAreAboveGround, &theyAreLit);

        if (theirOccupiedEdges & (1 << ((effectiveRotation + 2) % NumOrthogonalDirections)) && (weAreOwned && !theyAreOwned))
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

    uint32_t remap_base = SPRITE_ID_PALETTE_COLOUR_1(COLOUR_DARK_PURPLE);
    uint32_t remap_edge = SPRITE_ID_PALETTE_COLOUR_1(COLOUR_WHITE);
    uint32_t remap_lit = SPRITE_ID_PALETTE_COLOUR_1(COLOUR_DARK_BROWN);

    // Edges which are internal to objects (i.e., the tile on both sides
    //  is occupied/lit) are not rendered to provide visual clarity.
    uint8_t dullEdges = 0xF & ~occupiedEdges & ~litEdges;
    uint8_t paintEdges = ((weAreOccupied || weAreLit) && weAreOwned) ? ~dullEdges : 0xF;

    const auto virtualFloorOffset = CoordsXYZ{ 0, 0, _virtualFloorHeight };
    if (paintEdges & EDGE_NE)
    {
        PaintAddImageAsParent(
            session,
            SPR_G2_SELECTION_EDGE_NE
                | (!(occupiedEdges & EDGE_NE) ? ((litEdges & EDGE_NE) ? remap_lit : remap_base) : remap_edge),
            virtualFloorOffset, { 0, 0, 1 }, { 5, 5, _virtualFloorHeight + ((dullEdges & EDGE_NE) ? -2 : 0) });
    }
    if (paintEdges & EDGE_SE)
    {
        PaintAddImageAsParent(
            session,
            SPR_G2_SELECTION_EDGE_SE
                | (!(occupiedEdges & EDGE_SE) ? ((litEdges & EDGE_SE) ? remap_lit : remap_base) : remap_edge),
            virtualFloorOffset, { 1, 1, 1 }, { 16, 27, _virtualFloorHeight + ((dullEdges & EDGE_SE) ? -2 : 0) });
    }
    if (paintEdges & EDGE_SW)
    {
        PaintAddImageAsParent(
            session,
            SPR_G2_SELECTION_EDGE_SW
                | (!(occupiedEdges & EDGE_SW) ? ((litEdges & EDGE_SW) ? remap_lit : remap_base) : remap_edge),
            virtualFloorOffset, { 1, 1, 1 }, { 27, 16, _virtualFloorHeight + ((dullEdges & EDGE_SW) ? -2 : 0) });
    }
    if (paintEdges & EDGE_NW)
    {
        PaintAddImageAsParent(
            session,
            SPR_G2_SELECTION_EDGE_NW
                | (!(occupiedEdges & EDGE_NW) ? ((litEdges & EDGE_NW) ? remap_lit : remap_base) : remap_edge),
            virtualFloorOffset, { 0, 0, 1 }, { 5, 5, _virtualFloorHeight + ((dullEdges & EDGE_NW) ? -2 : 0) });
    }

    if (gConfigGeneral.virtual_floor_style != VirtualFloorStyles::Glassy)
        return;

    if (!weAreOccupied && !weAreLit && weAreAboveGround && weAreOwned)
    {
        int32_t imageColourFlats = SPR_G2_SURFACE_GLASSY_RECOLOURABLE | IMAGE_TYPE_REMAP | IMAGE_TYPE_TRANSPARENT
            | EnumValue(FilterPaletteID::PaletteWater) << 19;
        PaintAddImageAsParent(session, imageColourFlats, virtualFloorOffset, { 30, 30, 0 }, { 2, 2, _virtualFloorHeight - 3 });
    }
}

uint16_t virtual_floor_get_height()
{
    return _virtualFloorHeight;
}
