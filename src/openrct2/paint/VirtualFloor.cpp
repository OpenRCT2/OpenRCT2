/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "VirtualFloor.h"
#include "../Input.h"
#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../sprites.h"
#include "../world/Map.h"
#include "Paint.h"
#include "tile_element/Paint.TileElement.h"
#include "VirtualFloor.h"
#include <algorithm>
#include <limits>

static uint16_t        _virtualFloorBaseSize = 5 * 32;
static uint16_t        _virtualFloorHeight = 0;
static LocationXYZ16 _virtualFloorLastMinPos;
static LocationXYZ16 _virtualFloorLastMaxPos;
static uint32_t        _virtualFloorFlags = 0;

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
    _virtualFloorLastMinPos.x = std::numeric_limits<int16_t>::max();
    _virtualFloorLastMinPos.y = std::numeric_limits<int16_t>::max();
    _virtualFloorLastMaxPos.x = std::numeric_limits<int16_t>::lowest();
    _virtualFloorLastMaxPos.y = std::numeric_limits<int16_t>::lowest();
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
    // First, let's figure out how big our selection is.
    LocationXY16 min_position = { std::numeric_limits<int16_t>::max(),    std::numeric_limits<int16_t>::max() };
    LocationXY16 max_position = { std::numeric_limits<int16_t>::lowest(), std::numeric_limits<int16_t>::lowest() };

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
    {
        min_position = gMapSelectPositionA;
        max_position = gMapSelectPositionB;
    }

    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        for (LocationXY16 * tile = gMapSelectionTiles; tile->x != -1; tile++)
        {
            min_position.x = std::min(min_position.x, tile->x);
            min_position.y = std::min(min_position.y, tile->y);
            max_position.x = std::max(max_position.x, tile->x);
            max_position.y = std::max(max_position.y, tile->y);
        }
    }

    // Apply the virtual floor size to the computed invalidation area.
    min_position.x -= _virtualFloorBaseSize + 16;
    min_position.y -= _virtualFloorBaseSize + 16;
    max_position.x += _virtualFloorBaseSize + 16;
    max_position.y += _virtualFloorBaseSize + 16;

    // Invalidate previous region if appropriate.
    if (_virtualFloorLastMinPos.x != std::numeric_limits<int16_t>::max() &&
        _virtualFloorLastMinPos.y != std::numeric_limits<int16_t>::max() &&
        _virtualFloorLastMaxPos.x != std::numeric_limits<int16_t>::lowest() &&
        _virtualFloorLastMaxPos.y != std::numeric_limits<int16_t>::lowest())
    {
        LocationXY16 prevMins = { _virtualFloorLastMinPos.x, _virtualFloorLastMinPos.y };
        LocationXY16 prevMaxs = { _virtualFloorLastMaxPos.x, _virtualFloorLastMaxPos.y };

        if (prevMins.x != min_position.x ||
            prevMins.y != min_position.y ||
            prevMaxs.x != max_position.x ||
            prevMaxs.y != max_position.y ||
            (_virtualFloorFlags & VIRTUAL_FLOOR_FORCE_INVALIDATION) != 0)
        {
            log_verbose("Invalidating previous region, Min: %d %d, Max: %d %d",
                prevMins.x, prevMins.y, prevMaxs.x, prevMaxs.y);
            map_invalidate_region(prevMins, prevMaxs);
        }
    }

    // Do not invalidate new region if floor hasn't moved.
    if (_virtualFloorLastMinPos.x == min_position.x &&
        _virtualFloorLastMinPos.y == min_position.y &&
        _virtualFloorLastMinPos.z == _virtualFloorHeight)
    {
        return;
    }

    if (!(_virtualFloorFlags & VIRTUAL_FLOOR_FLAG_ENABLED))
    {
        return;
    }

    log_verbose("Min: %d %d, Max: %d %d\n", min_position.x, min_position.y, max_position.x, max_position.y);

    // Invalidate new region if coordinates are set.
    if (min_position.x != std::numeric_limits<int16_t>::max() &&
        min_position.y != std::numeric_limits<int16_t>::max() &&
        max_position.x != std::numeric_limits<int16_t>::lowest() &&
        max_position.y != std::numeric_limits<int16_t>::lowest())
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

bool virtual_floor_tile_is_floor(int16_t x, int16_t y)
{
    if (!virtual_floor_is_enabled())
    {
        return false;
    }

    // Check if map selection (usually single tiles) are enabled
    //  and if the current tile is near or on them
    if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE) &&
        x >= gMapSelectPositionA.x - _virtualFloorBaseSize &&
        y >= gMapSelectPositionA.y - _virtualFloorBaseSize &&
        x <= gMapSelectPositionB.x + _virtualFloorBaseSize &&
        y <= gMapSelectPositionB.y + _virtualFloorBaseSize)
    {
        return true;
    }
    else if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        // Check if we are anywhere near the selection tiles (larger scenery / rides)
        for (LocationXY16 * tile = gMapSelectionTiles; tile->x != -1; tile++)
        {
            if (x >= tile->x - _virtualFloorBaseSize &&
                y >= tile->y - _virtualFloorBaseSize &&
                x <= tile->x + _virtualFloorBaseSize &&
                y <= tile->y + _virtualFloorBaseSize)
            {
                return true;
            }
        }
    }

    return false;
}

static void virtual_floor_get_tile_properties(int16_t x, int16_t y, int16_t height, bool * outOccupied, uint8_t * outOccupiedEdges, bool * outBelowGround, bool * outLit)
{
    *outOccupied        = false;
    *outOccupiedEdges   = 0;
    *outBelowGround     = false;
    *outLit             = false;

    // See if we are a selected tile
    if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
    {
        if (x >= gMapSelectPositionA.x &&
            y >= gMapSelectPositionA.y &&
            x <= gMapSelectPositionB.x &&
            y <= gMapSelectPositionB.y)
        {
            *outLit = true;
        }
    }

    // See if we are on top of the selection tiles
    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        for (LocationXY16 * tile = gMapSelectionTiles; tile->x != -1; tile++)
        {
            if (x == tile->x &&
                y == tile->y)
            {
                *outLit = true;
                break;
            }
        }
    }

    // Iterate through the map elements of the current tile to find:
    //  * Surfaces, which may put us underground
    //  * Walls / banners, which are displayed as occupied edges
    //  * Ghost objects, which are displayed as lit squares
    rct_tile_element * tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        int32_t elementType = tileElement->GetType();

        if (elementType == TILE_ELEMENT_TYPE_SURFACE)
        {
            if (height < tileElement->clearance_height)
            {
                *outBelowGround    = true;
            }
            else if (height < tileElement->base_height + 2 &&
                 tileElement->properties.surface.slope != 0)
            {
                *outBelowGround    = true;
                *outOccupied       = true;
            }
            continue;
        }

        if (height >= tileElement->clearance_height ||
            height < tileElement->base_height)
        {
            continue;
        }

        if (elementType == TILE_ELEMENT_TYPE_WALL ||
            elementType == TILE_ELEMENT_TYPE_BANNER)
        {
            int32_t direction = tile_element_get_direction(tileElement);
            *outOccupiedEdges   |= 1 << direction;
            continue;
        }

        if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
        {
            *outLit = true;
            continue;
        }

        *outOccupied = true;
    }
    while (!(tileElement++)->IsLastForTile());
}

void virtual_floor_paint(paint_session * session)
{
    static constexpr const CoordsXY scenery_half_tile_offsets[4] =
    {
        { -32,   0 },
        {   0,  32 },
        {  32,   0 },
        {   0, -32 }
    };

    if (_virtualFloorHeight < MINIMUM_LAND_HEIGHT)
        return;

    uint8_t direction = session->CurrentRotation;

    // This is a virtual floor, so no interactions
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;

    int16_t virtualFloorClipHeight = _virtualFloorHeight / 8;

    // Check for occupation and walls
    bool    weAreOccupied;
    uint8_t   occupiedEdges;
    bool    weAreBelowGround;
    bool    weAreLit;
    uint8_t   litEdges = 0;

    virtual_floor_get_tile_properties(session->MapPosition.x, session->MapPosition.y, virtualFloorClipHeight, &weAreOccupied, &occupiedEdges, &weAreBelowGround, &weAreLit);

    // Move the bits around to match the current rotation
    occupiedEdges |= occupiedEdges << 4;
    occupiedEdges >>= (4 - direction);
    occupiedEdges &= 0x0F;

    // Try the four tiles next to us for the same parameters as above,
    //  if our parameters differ we set an edge towards that tile
    for (uint8_t i = 0; i < 4; i++)
    {
        uint8_t effectiveRotation = (4 + i - direction) % 4;
        int16_t theirLocationX = session->MapPosition.x + scenery_half_tile_offsets[effectiveRotation].x;
        int16_t theirLocationY = session->MapPosition.y + scenery_half_tile_offsets[effectiveRotation].y;

        bool    theyAreOccupied;
        uint8_t   theirOccupiedEdges;
        bool    theyAreBelowGround;
        bool    theyAreLit;

        virtual_floor_get_tile_properties(theirLocationX, theirLocationY, virtualFloorClipHeight, &theyAreOccupied, &theirOccupiedEdges, &theyAreBelowGround, &theyAreLit);

        if (theirOccupiedEdges & (1 << ((effectiveRotation + 2) % 4)))
        {
            occupiedEdges   |= 1 << i;
        }
        if (weAreLit != theyAreLit)
        {
            litEdges        |= 1 << i;
        }
        else if (weAreOccupied != theyAreOccupied ||
                 weAreBelowGround != theyAreBelowGround)
        {
            occupiedEdges   |= 1 << i;
        }
    }

    uint32_t remap_base = COLOUR_DARK_PURPLE << 19 | IMAGE_TYPE_REMAP;
    uint32_t remap_edge = COLOUR_WHITE << 19 | IMAGE_TYPE_REMAP;
    uint32_t remap_lit  = COLOUR_DARK_BROWN << 19 | IMAGE_TYPE_REMAP;

    // Edges which are internal to objects (i.e., the tile on both sides
    //  is occupied/lit) are not rendered to provide visual clarity.
    uint8_t dullEdges = 0xF & ~occupiedEdges & ~litEdges;
    uint8_t paintEdges = (weAreOccupied || weAreLit)? ~dullEdges : 0xF;

    if (paintEdges & 0x1)
    {
        sub_98197C(
            session,
            SPR_G2_SELECTION_EDGE_NE | (!(occupiedEdges & 0x1) ? ((litEdges & 0x1) ? remap_lit : remap_base) : remap_edge), 0,
            0, 0, 0, 1, _virtualFloorHeight, 5, 5, _virtualFloorHeight + ((dullEdges & 0x1) ? -2 : 0));
    }
    if (paintEdges & 0x2)
    {
        sub_98197C(
            session,
            SPR_G2_SELECTION_EDGE_SE | (!(occupiedEdges & 0x2) ? ((litEdges & 0x2) ? remap_lit : remap_base) : remap_edge), 0,
            0, 1, 1, 1, _virtualFloorHeight, 16, 27, _virtualFloorHeight + ((dullEdges & 0x2) ? -2 : 0));
    }
    if (paintEdges & 0x4)
    {
        sub_98197C(
            session,
            SPR_G2_SELECTION_EDGE_SW | (!(occupiedEdges & 0x4) ? ((litEdges & 0x4) ? remap_lit : remap_base) : remap_edge), 0,
            0, 1, 1, 1, _virtualFloorHeight, 27, 16, _virtualFloorHeight + ((dullEdges & 0x4) ? -2 : 0));
    }
    if (paintEdges & 0x8)
    {
        sub_98197C(
            session,
            SPR_G2_SELECTION_EDGE_NW | (!(occupiedEdges & 0x8) ? ((litEdges & 0x8) ? remap_lit : remap_base) : remap_edge), 0,
            0, 0, 0, 1, _virtualFloorHeight, 5, 5, _virtualFloorHeight + ((dullEdges & 0x8) ? -2 : 0));
    }

    if (gConfigGeneral.virtual_floor_style != VIRTUAL_FLOOR_STYLE_GLASSY)
        return;

    if (!weAreOccupied && !weAreLit)
    {
        int32_t imageColourFlats = SPR_G2_SURFACE_GLASSY_RECOLOURABLE | IMAGE_TYPE_REMAP | IMAGE_TYPE_TRANSPARENT | PALETTE_WATER << 19;
        sub_98197C(session, imageColourFlats, 0, 0, 30, 30, 0, _virtualFloorHeight, 2, 2, _virtualFloorHeight - 3);
    }
}

uint16_t virtual_floor_get_height()
{
    return _virtualFloorHeight;
}
