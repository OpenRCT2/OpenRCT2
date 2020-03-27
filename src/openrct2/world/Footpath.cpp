/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../actions/FootpathPlaceAction.hpp"
#include "../actions/FootpathRemoveAction.hpp"
#include "../actions/LandSetRightsAction.hpp"
#include "../core/Guard.hpp"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/FootpathObject.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../paint/VirtualFloor.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../util/Util.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Sprite.h"
#include "Surface.h"

#include <algorithm>
#include <iterator>

void footpath_update_queue_entrance_banner(const CoordsXY& footpathPos, TileElement* tileElement);

uint8_t gFootpathProvisionalFlags;
CoordsXYZ gFootpathProvisionalPosition;
uint8_t gFootpathProvisionalType;
uint8_t gFootpathProvisionalSlope;
uint8_t gFootpathConstructionMode;
uint16_t gFootpathSelectedId;
uint8_t gFootpathSelectedType;
CoordsXYZ gFootpathConstructFromPosition;
uint8_t gFootpathConstructDirection;
uint8_t gFootpathConstructSlope;
uint8_t gFootpathConstructValidDirections;
money32 gFootpathPrice;
uint8_t gFootpathGroundFlags;

static uint8_t* _footpathQueueChainNext;
static uint8_t _footpathQueueChain[64];

// This is the coordinates that a user of the bin should move to
// rct2: 0x00992A4C
const CoordsXY BinUseOffsets[4] = {
    { 11, 16 },
    { 16, 21 },
    { 21, 16 },
    { 16, 11 },
};

// These are the offsets for bench positions on footpaths, 2 for each edge
// rct2: 0x00981F2C, 0x00981F2E
const CoordsXY BenchUseOffsets[8] = {
    { 7, 12 }, { 12, 25 }, { 25, 20 }, { 20, 7 }, { 7, 20 }, { 20, 25 }, { 25, 12 }, { 12, 7 },
};

/** rct2: 0x00981D6C, 0x00981D6E */
const CoordsXY DirectionOffsets[4] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };

// rct2: 0x0097B974
static constexpr const uint16_t EntranceDirections[] = {
    (4),     0, 0, 0, 0, 0, 0, 0, // ENTRANCE_TYPE_RIDE_ENTRANCE,
    (4),     0, 0, 0, 0, 0, 0, 0, // ENTRANCE_TYPE_RIDE_EXIT,
    (4 | 1), 0, 0, 0, 0, 0, 0, 0, // ENTRANCE_TYPE_PARK_ENTRANCE
};

/** rct2: 0x0098D7F0 */
static constexpr const uint8_t connected_path_count[] = {
    0, // 0b0000
    1, // 0b0001
    1, // 0b0010
    2, // 0b0011
    1, // 0b0100
    2, // 0b0101
    2, // 0b0110
    3, // 0b0111
    1, // 0b1000
    2, // 0b1001
    2, // 0b1010
    3, // 0b1011
    2, // 0b1100
    3, // 0b1101
    3, // 0b1110
    4, // 0b1111
};

static const uint8_t AllWideGroupDirections[][8] = { { 2, 1, 0, 3, 7, 6, 5, 4 }, { 1, 2, 3, 0, 7, 4, 5, 6 },
                                                     { 3, 2, 1, 0, 4, 7, 6, 5 }, { 2, 3, 0, 1, 4, 5, 6, 7 },
                                                     { 0, 3, 2, 1, 5, 4, 7, 6 }, { 3, 0, 1, 2, 5, 6, 7, 4 },
                                                     { 1, 0, 3, 2, 6, 5, 4, 7 }, { 0, 1, 2, 3, 6, 7, 4, 5 } };

static const uint8_t AllWideGroupConnections[][8] = { { 32, 8, 2, 128, 16, 4, 1, 64 }, { 8, 32, 128, 2, 16, 64, 1, 4 },
                                                      { 128, 32, 8, 2, 64, 16, 4, 1 }, { 32, 128, 2, 8, 64, 1, 4, 16 },
                                                      { 2, 128, 32, 8, 1, 64, 16, 4 }, { 128, 2, 8, 32, 1, 4, 16, 64 },
                                                      { 8, 2, 128, 32, 4, 1, 64, 16 }, { 2, 8, 32, 128, 4, 16, 64, 1 } };

static const uint8_t AllWideGroupEdges[][4] = { { 4, 2, 1, 8 }, { 2, 4, 8, 1 }, { 8, 4, 2, 1 }, { 4, 8, 1, 2 },
                                                { 1, 8, 4, 2 }, { 8, 1, 2, 4 }, { 2, 1, 8, 4 }, { 1, 2, 4, 8 } };

int32_t entrance_get_directions(const TileElement* tileElement)
{
    uint8_t entranceType = tileElement->AsEntrance()->GetEntranceType();
    uint8_t sequence = tileElement->AsEntrance()->GetSequenceIndex();
    return EntranceDirections[(entranceType * 8) + sequence];
}

static bool entrance_has_direction(TileElement* tileElement, int32_t direction)
{
    return entrance_get_directions(tileElement) & (1 << (direction & 3));
}

TileElement* map_get_footpath_element(const CoordsXYZ& coords)
{
    TileElement* tileElement = map_get_first_element_at(coords);
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH && tileElement->GetBaseZ() == coords.z)
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

money32 footpath_remove(const CoordsXYZ& footpathLoc, int32_t flags)
{
    auto action = FootpathRemoveAction(footpathLoc);
    action.SetFlags(flags);

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        auto res = GameActions::Execute(&action);
        return res->Cost;
    }
    auto res = GameActions::Query(&action);
    return res->Cost;
}

/**
 *
 *  rct2: 0x006A76FF
 */
money32 footpath_provisional_set(int32_t type, const CoordsXYZ& footpathLoc, int32_t slope)
{
    money32 cost;

    footpath_provisional_remove();

    auto footpathPlaceAction = FootpathPlaceAction(footpathLoc, slope, type);
    footpathPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
    auto res = GameActions::Execute(&footpathPlaceAction);
    cost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
    if (res->Error == GA_ERROR::OK)
    {
        gFootpathProvisionalType = type;
        gFootpathProvisionalPosition = footpathLoc;
        gFootpathProvisionalSlope = slope;
        gFootpathProvisionalFlags |= PROVISIONAL_PATH_FLAG_1;

        if (gFootpathGroundFlags & ELEMENT_IS_UNDERGROUND)
        {
            viewport_set_visibility(1);
        }
        else
        {
            viewport_set_visibility(3);
        }
    }

    // Invalidate previous footpath piece.
    virtual_floor_invalidate();

    if (!scenery_tool_is_active())
    {
        if (res->Error != GA_ERROR::OK)
        {
            // If we can't build this, don't show a virtual floor.
            virtual_floor_set_height(0);
        }
        else if (
            gFootpathConstructSlope == TILE_ELEMENT_SLOPE_FLAT
            || gFootpathProvisionalPosition.z < gFootpathConstructFromPosition.z)
        {
            // Going either straight on, or down.
            virtual_floor_set_height(gFootpathProvisionalPosition.z);
        }
        else
        {
            // Going up in the world!
            virtual_floor_set_height(gFootpathProvisionalPosition.z + LAND_HEIGHT_STEP);
        }
    }

    return cost;
}

/**
 *
 *  rct2: 0x006A77FF
 */
void footpath_provisional_remove()
{
    if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1)
    {
        gFootpathProvisionalFlags &= ~PROVISIONAL_PATH_FLAG_1;

        footpath_remove(
            gFootpathProvisionalPosition,
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                | GAME_COMMAND_FLAG_GHOST);
    }
}

/**
 *
 *  rct2: 0x006A7831
 */
void footpath_provisional_update()
{
    if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_SHOW_ARROW)
    {
        gFootpathProvisionalFlags &= ~PROVISIONAL_PATH_FLAG_SHOW_ARROW;

        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
        map_invalidate_tile_full(gFootpathConstructFromPosition);
    }
    footpath_provisional_remove();
}

/**
 * Determines the location of the footpath at which we point with the cursor. If no footpath is underneath the cursor,
 * then return the location of the ground tile. Besides the location it also computes the direction of the yellow arrow
 * when we are going to build a footpath bridge/tunnel.
 *  rct2: 0x00689726
 *  In:
 *      screenX: eax
 *      screenY: ebx
 *  Out:
 *      x: ax
 *      y: bx
 *      direction: ecx
 *      tileElement: edx
 */
CoordsXY footpath_get_coordinates_from_pos(const ScreenCoordsXY& screenCoords, int32_t* direction, TileElement** tileElement)
{
    int32_t z = 0, interactionType;
    TileElement* myTileElement;
    rct_viewport* viewport;
    CoordsXY position = {};

    get_map_coordinates_from_pos(
        screenCoords, VIEWPORT_INTERACTION_MASK_FOOTPATH, position, &interactionType, &myTileElement, &viewport);
    if (interactionType != VIEWPORT_INTERACTION_ITEM_FOOTPATH
        || !(viewport->flags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL)))
    {
        get_map_coordinates_from_pos(
            screenCoords, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, position, &interactionType,
            &myTileElement, &viewport);
        if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE)
        {
            position.setNull();
            return position;
        }
    }

    auto minPosition = position;
    auto maxPosition = position + CoordsXY{ 31, 31 };

    position += CoordsXY{ 16, 16 };

    if (interactionType == VIEWPORT_INTERACTION_ITEM_FOOTPATH)
    {
        z = myTileElement->GetBaseZ();
        if (myTileElement->AsPath()->IsSloped())
        {
            z += 8;
        }
    }

    auto start_vp_pos = screen_coord_to_viewport_coord(viewport, screenCoords);

    for (int32_t i = 0; i < 5; i++)
    {
        if (interactionType != VIEWPORT_INTERACTION_ITEM_FOOTPATH)
        {
            z = tile_element_height(position);
        }
        position = viewport_coord_to_map_coord(start_vp_pos.x, start_vp_pos.y, z);
        position.x = std::clamp(position.x, minPosition.x, maxPosition.x);
        position.y = std::clamp(position.y, minPosition.y, maxPosition.y);
    }

    // Determine to which edge the cursor is closest
    uint32_t myDirection;
    int32_t mod_x = position.x & 0x1F, mod_y = position.y & 0x1F;
    if (mod_x < mod_y)
    {
        if (mod_x + mod_y < 32)
        {
            myDirection = 0;
        }
        else
        {
            myDirection = 1;
        }
    }
    else
    {
        if (mod_x + mod_y < 32)
        {
            myDirection = 3;
        }
        else
        {
            myDirection = 2;
        }
    }

    position = position.ToTileStart();

    if (direction != nullptr)
        *direction = myDirection;
    if (tileElement != nullptr)
        *tileElement = myTileElement;

    return position;
}

/**
 *
 *  rct2: 0x0068A0C9
 * screenX: eax
 * screenY: ebx
 * x: ax
 * y: bx
 * direction: cl
 * tileElement: edx
 */
CoordsXY footpath_bridge_get_info_from_pos(const ScreenCoordsXY& screenCoords, int32_t* direction, TileElement** tileElement)
{
    // First check if we point at an entrance or exit. In that case, we would want the path coming from the entrance/exit.
    int32_t interactionType;
    rct_viewport* viewport;

    CoordsXY map_pos = {};
    get_map_coordinates_from_pos(
        screenCoords, VIEWPORT_INTERACTION_MASK_RIDE, map_pos, &interactionType, tileElement, &viewport);

    if (interactionType == VIEWPORT_INTERACTION_ITEM_RIDE
        && viewport->flags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL)
        && (*tileElement)->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
    {
        int32_t directions = entrance_get_directions(*tileElement);
        if (directions & 0x0F)
        {
            int32_t bx = bitscanforward(directions);
            bx += (*tileElement)->AsEntrance()->GetDirection();
            bx &= 3;
            if (direction != nullptr)
                *direction = bx;
            return map_pos;
        }
    }

    get_map_coordinates_from_pos(
        screenCoords, VIEWPORT_INTERACTION_MASK_RIDE & VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN,
        map_pos, &interactionType, tileElement, &viewport);
    if (interactionType == VIEWPORT_INTERACTION_ITEM_RIDE && (*tileElement)->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
    {
        int32_t directions = entrance_get_directions(*tileElement);
        if (directions & 0x0F)
        {
            int32_t bx = (*tileElement)->GetDirectionWithOffset(bitscanforward(directions));
            if (direction != nullptr)
                *direction = bx;
            return map_pos;
        }
    }

    // We point at something else
    return footpath_get_coordinates_from_pos(screenCoords, direction, tileElement);
}

/**
 *
 *  rct2: 0x00673883
 */
void footpath_remove_litter(const CoordsXYZ& footpathPos)
{
    uint16_t spriteIndex = sprite_get_first_in_quadrant(footpathPos.x, footpathPos.y);
    while (spriteIndex != SPRITE_INDEX_NULL)
    {
        Litter* sprite = &get_sprite(spriteIndex)->litter;
        uint16_t nextSpriteIndex = sprite->next_in_quadrant;
        if (sprite->linked_list_index == SPRITE_LIST_LITTER)
        {
            int32_t distanceZ = abs(sprite->z - footpathPos.z);
            if (distanceZ <= 32)
            {
                invalidate_sprite_0(sprite);
                sprite_remove(sprite);
            }
        }
        spriteIndex = nextSpriteIndex;
    }
}

/**
 *
 *  rct2: 0x0069A48B
 */
void footpath_interrupt_peeps(const CoordsXYZ& footpathPos)
{
    uint16_t spriteIndex = sprite_get_first_in_quadrant(footpathPos.x, footpathPos.y);
    while (spriteIndex != SPRITE_INDEX_NULL)
    {
        Peep* peep = &get_sprite(spriteIndex)->peep;
        uint16_t nextSpriteIndex = peep->next_in_quadrant;
        if (peep->linked_list_index == SPRITE_LIST_PEEP)
        {
            if (peep->state == PEEP_STATE_SITTING || peep->state == PEEP_STATE_WATCHING)
            {
                if (peep->z == footpathPos.z)
                {
                    peep->SetState(PEEP_STATE_WALKING);
                    peep->destination_x = (peep->x & 0xFFE0) + 16;
                    peep->destination_y = (peep->y & 0xFFE0) + 16;
                    peep->destination_tolerance = 5;
                    peep->UpdateCurrentActionSpriteType();
                }
            }
        }
        spriteIndex = nextSpriteIndex;
    }
}

/**
 * Returns true if the edge of tile x, y specified by direction is occupied by a fence
 * between heights z0 and z1.
 *
 * Note that there may still be a fence on the opposing tile.
 *
 *  rct2: 0x006E59DC
 */
bool fence_in_the_way(const CoordsXYRangedZ& fencePos, int32_t direction)
{
    TileElement* tileElement;

    tileElement = map_get_first_element_at(fencePos);
    if (tileElement == nullptr)
        return false;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (tileElement->IsGhost())
            continue;
        if (fencePos.baseZ >= tileElement->GetClearanceZ())
            continue;
        if (fencePos.clearanceZ <= tileElement->GetBaseZ())
            continue;
        if ((tileElement->GetDirection()) != direction)
            continue;

        return true;
    } while (!(tileElement++)->IsLastForTile());
    return false;
}

static PathElement* footpath_connect_corners_get_neighbour(const CoordsXYZ& footpathPos, int32_t requireEdges)
{
    if (!map_is_location_valid(footpathPos))
    {
        return nullptr;
    }

    TileElement* tileElement = map_get_first_element_at(footpathPos);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        auto pathElement = tileElement->AsPath();
        if (pathElement->IsQueue())
            continue;
        if (tileElement->GetBaseZ() != footpathPos.z)
            continue;
        if (!(pathElement->GetEdgesAndCorners() & requireEdges))
            continue;

        return pathElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 * Sets the corner edges of four path tiles.
 * The function will search for a path in the direction given, then check clockwise to see if it there is a path and again until
 * it reaches the initial path. In other words, checks if there are four paths together so that it can set the inner corners of
 * each one.
 *
 *  rct2: 0x006A70EB
 */
static void footpath_connect_corners(const CoordsXY& footpathPos, PathElement* initialTileElement)
{
    using PathElementCoordsPair = std::pair<PathElement*, CoordsXY>;
    std::array<PathElementCoordsPair, 4> tileElements;

    if (initialTileElement->IsQueue())
        return;
    if (initialTileElement->IsSloped())
        return;

    tileElements[0] = { initialTileElement, footpathPos };
    int32_t z = initialTileElement->GetBaseZ();
    for (int32_t initialDirection = 0; initialDirection < 4; initialDirection++)
    {
        int32_t direction = initialDirection;
        auto currentPos = footpathPos + CoordsDirectionDelta[direction];

        tileElements[1] = { footpath_connect_corners_get_neighbour({ currentPos, z }, (1 << direction_reverse(direction))),
                            currentPos };
        if (tileElements[1].first == nullptr)
            continue;

        direction = direction_next(direction);
        currentPos += CoordsDirectionDelta[direction];
        tileElements[2] = { footpath_connect_corners_get_neighbour({ currentPos, z }, (1 << direction_reverse(direction))),
                            currentPos };
        if (tileElements[2].first == nullptr)
            continue;

        direction = direction_next(direction);
        currentPos += CoordsDirectionDelta[direction];
        // First check link to previous tile
        tileElements[3] = { footpath_connect_corners_get_neighbour({ currentPos, z }, (1 << direction_reverse(direction))),
                            currentPos };
        if (tileElements[3].first == nullptr)
            continue;
        // Second check link to initial tile
        tileElements[3] = { footpath_connect_corners_get_neighbour({ currentPos, z }, (1 << ((direction + 1) & 3))),
                            currentPos };
        if (tileElements[3].first == nullptr)
            continue;

        direction = direction_next(direction);
        tileElements[3].first->SetCorners(tileElements[3].first->GetCorners() | (1 << (direction)));
        map_invalidate_element(tileElements[3].second, reinterpret_cast<TileElement*>(tileElements[3].first));

        direction = direction_prev(direction);
        tileElements[2].first->SetCorners(tileElements[2].first->GetCorners() | (1 << (direction)));

        map_invalidate_element(tileElements[2].second, reinterpret_cast<TileElement*>(tileElements[2].first));

        direction = direction_prev(direction);
        tileElements[1].first->SetCorners(tileElements[1].first->GetCorners() | (1 << (direction)));

        map_invalidate_element(tileElements[1].second, reinterpret_cast<TileElement*>(tileElements[1].first));

        direction = initialDirection;
        tileElements[0].first->SetCorners(tileElements[0].first->GetCorners() | (1 << (direction)));
        map_invalidate_element(tileElements[0].second, reinterpret_cast<TileElement*>(tileElements[0].first));
    }
}

struct rct_neighbour
{
    uint8_t order;
    uint8_t direction;
    uint8_t ride_index;
    uint8_t entrance_index;
};

struct rct_neighbour_list
{
    rct_neighbour items[8];
    size_t count;
};

static int32_t rct_neighbour_compare(const void* a, const void* b)
{
    uint8_t va = ((rct_neighbour*)a)->order;
    uint8_t vb = ((rct_neighbour*)b)->order;
    if (va < vb)
        return 1;
    else if (va > vb)
        return -1;
    else
    {
        uint8_t da = ((rct_neighbour*)a)->direction;
        uint8_t db = ((rct_neighbour*)b)->direction;
        if (da < db)
            return -1;
        else if (da > db)
            return 1;
        else
            return 0;
    }
}

static void neighbour_list_init(rct_neighbour_list* neighbourList)
{
    neighbourList->count = 0;
}

static void neighbour_list_push(
    rct_neighbour_list* neighbourList, int32_t order, int32_t direction, ride_id_t rideIndex, uint8_t entrance_index)
{
    Guard::Assert(neighbourList->count < std::size(neighbourList->items));
    neighbourList->items[neighbourList->count].order = order;
    neighbourList->items[neighbourList->count].direction = direction;
    neighbourList->items[neighbourList->count].ride_index = rideIndex;
    neighbourList->items[neighbourList->count].entrance_index = entrance_index;
    neighbourList->count++;
}

static bool neighbour_list_pop(rct_neighbour_list* neighbourList, rct_neighbour* outNeighbour)
{
    if (neighbourList->count == 0)
        return false;

    *outNeighbour = neighbourList->items[0];
    const size_t bytesToMove = (neighbourList->count - 1) * sizeof(neighbourList->items[0]);
    memmove(&neighbourList->items[0], &neighbourList->items[1], bytesToMove);
    neighbourList->count--;
    return true;
}

static void neighbour_list_remove(rct_neighbour_list* neighbourList, size_t index)
{
    Guard::ArgumentInRange<size_t>(index, 0, neighbourList->count - 1);
    int32_t itemsRemaining = (int32_t)(neighbourList->count - index) - 1;
    if (itemsRemaining > 0)
    {
        memmove(&neighbourList->items[index], &neighbourList->items[index + 1], sizeof(rct_neighbour) * itemsRemaining);
    }
    neighbourList->count--;
}

static void neighbour_list_sort(rct_neighbour_list* neighbourList)
{
    qsort(neighbourList->items, neighbourList->count, sizeof(rct_neighbour), rct_neighbour_compare);
}

static TileElement* footpath_get_element(const CoordsXYRangedZ& footpathPos, int32_t direction)
{
    TileElement* tileElement;
    int32_t slope;

    tileElement = map_get_first_element_at(footpathPos);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (footpathPos.clearanceZ == tileElement->GetBaseZ())
        {
            if (tileElement->AsPath()->IsSloped())
            {
                slope = tileElement->AsPath()->GetSlopeDirection();
                if (slope != direction)
                    break;
            }
            return tileElement;
        }
        if (footpathPos.baseZ == tileElement->GetBaseZ())
        {
            if (!tileElement->AsPath()->IsSloped())
                break;

            slope = direction_reverse(tileElement->AsPath()->GetSlopeDirection());
            if (slope != direction)
                break;

            return tileElement;
        }
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

/**
 * Attempt to connect a newly disconnected queue tile to the specified path tile
 */
static bool footpath_reconnect_queue_to_path(
    const CoordsXY& footpathPos, TileElement* tileElement, int32_t action, int32_t direction)
{
    if (((tileElement->AsPath()->GetEdges() & (1 << direction)) == 0) ^ (action < 0))
        return false;

    auto targetQueuePos = footpathPos + CoordsDirectionDelta[direction];

    if (action < 0)
    {
        if (fence_in_the_way({ footpathPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() }, direction))
            return false;

        if (fence_in_the_way(
                { targetQueuePos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() }, direction_reverse(direction)))
            return false;
    }

    int32_t z = tileElement->GetBaseZ();
    TileElement* targetFootpathElement = footpath_get_element({ targetQueuePos, z - LAND_HEIGHT_STEP, z }, direction);
    if (targetFootpathElement != nullptr && !targetFootpathElement->AsPath()->IsQueue())
    {
        auto targetQueueElement = targetFootpathElement->AsPath();
        tileElement->AsPath()->SetSlopeDirection(0);
        if (action > 0)
        {
            tileElement->AsPath()->SetEdges(tileElement->AsPath()->GetEdges() & ~(1 << direction));
            targetQueueElement->SetEdges(targetQueueElement->GetEdges() & ~(1 << (direction_reverse(direction) & 3)));
            if (action >= 2)
                tileElement->AsPath()->SetSlopeDirection(direction);
        }
        else if (action < 0)
        {
            tileElement->AsPath()->SetEdges(tileElement->AsPath()->GetEdges() | (1 << direction));
            targetQueueElement->SetEdges(targetQueueElement->GetEdges() | (1 << (direction_reverse(direction) & 3)));
        }
        if (action != 0)
            map_invalidate_tile_full(targetQueuePos);
        return true;
    }
    return false;
}

static bool footpath_disconnect_queue_from_path(const CoordsXY& footpathPos, TileElement* tileElement, int32_t action)
{
    if (!tileElement->AsPath()->IsQueue())
        return false;

    if (tileElement->AsPath()->IsSloped())
        return false;

    uint8_t c = connected_path_count[tileElement->AsPath()->GetEdges()];
    if ((action < 0) ? (c >= 2) : (c < 2))
        return false;

    if (action < 0)
    {
        uint8_t direction = tileElement->AsPath()->GetSlopeDirection();
        if (footpath_reconnect_queue_to_path(footpathPos, tileElement, action, direction))
            return true;
    }

    for (Direction direction : ALL_DIRECTIONS)
    {
        if ((action < 0) && (direction == tileElement->AsPath()->GetSlopeDirection()))
            continue;
        if (footpath_reconnect_queue_to_path(footpathPos, tileElement, action, direction))
            return true;
    }

    return false;
}

/**
 *
 *  rct2: 0x006A6D7E
 */
static void loc_6A6D7E(
    const CoordsXYZ& initialTileElementPos, int32_t direction, TileElement* initialTileElement, int32_t flags, bool query,
    rct_neighbour_list* neighbourList)
{
    auto targetPos = CoordsXY{ initialTileElementPos } + CoordsDirectionDelta[direction];
    if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && map_is_edge(targetPos))
    {
        if (query)
        {
            neighbour_list_push(neighbourList, 7, direction, 255, 255);
        }
    }
    else
    {
        TileElement* tileElement = map_get_first_element_at(targetPos);
        if (tileElement == nullptr)
            return;
        do
        {
            switch (tileElement->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                    if (initialTileElementPos.z == tileElement->GetBaseZ())
                    {
                        if (tileElement->AsPath()->IsSloped() && tileElement->AsPath()->GetSlopeDirection() != direction)
                        {
                            return;
                        }
                        else
                        {
                            goto loc_6A6F1F;
                        }
                    }
                    if (initialTileElementPos.z - LAND_HEIGHT_STEP == tileElement->GetBaseZ())
                    {
                        if (!tileElement->AsPath()->IsSloped()
                            || tileElement->AsPath()->GetSlopeDirection() != direction_reverse(direction))
                        {
                            return;
                        }
                        goto loc_6A6F1F;
                    }
                    break;
                case TILE_ELEMENT_TYPE_TRACK:
                    if (initialTileElementPos.z == tileElement->GetBaseZ())
                    {
                        auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
                        if (ride == nullptr || !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                        {
                            continue;
                        }

                        const auto trackType = tileElement->AsTrack()->GetTrackType();
                        const uint8_t trackSequence = tileElement->AsTrack()->GetSequenceIndex();
                        if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH))
                        {
                            return;
                        }
                        uint16_t dx = direction_reverse(
                            (direction - tileElement->GetDirection()) & TILE_ELEMENT_DIRECTION_MASK);
                        if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & (1 << dx)))
                        {
                            return;
                        }
                        if (query)
                        {
                            neighbour_list_push(neighbourList, 1, direction, tileElement->AsTrack()->GetRideIndex(), 255);
                        }
                        goto loc_6A6FD2;
                    }
                    break;
                case TILE_ELEMENT_TYPE_ENTRANCE:
                    if (initialTileElementPos.z == tileElement->GetBaseZ())
                    {
                        if (entrance_has_direction(tileElement, direction_reverse(direction - tileElement->GetDirection())))
                        {
                            if (query)
                            {
                                neighbour_list_push(
                                    neighbourList, 8, direction, tileElement->AsEntrance()->GetRideIndex(),
                                    tileElement->AsEntrance()->GetStationIndex());
                            }
                            else
                            {
                                if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                                {
                                    footpath_queue_chain_push(tileElement->AsEntrance()->GetRideIndex());
                                }
                            }
                            goto loc_6A6FD2;
                        }
                    }
                    break;
            }
        } while (!(tileElement++)->IsLastForTile());
        return;

    loc_6A6F1F:
        if (query)
        {
            if (fence_in_the_way(
                    { targetPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() }, direction_reverse(direction)))
            {
                return;
            }
            if (tileElement->AsPath()->IsQueue())
            {
                if (connected_path_count[tileElement->AsPath()->GetEdges()] < 2)
                {
                    neighbour_list_push(
                        neighbourList, 4, direction, tileElement->AsPath()->GetRideIndex(),
                        tileElement->AsPath()->GetStationIndex());
                }
                else
                {
                    if ((initialTileElement)->GetType() == TILE_ELEMENT_TYPE_PATH && initialTileElement->AsPath()->IsQueue())
                    {
                        if (footpath_disconnect_queue_from_path(targetPos, tileElement, 0))
                        {
                            neighbour_list_push(
                                neighbourList, 3, direction, tileElement->AsPath()->GetRideIndex(),
                                tileElement->AsPath()->GetStationIndex());
                        }
                    }
                }
            }
            else
            {
                neighbour_list_push(neighbourList, 2, direction, 255, 255);
            }
        }
        else
        {
            footpath_disconnect_queue_from_path(targetPos, tileElement, 1 + ((flags >> 6) & 1));
            tileElement->AsPath()->SetEdges(tileElement->AsPath()->GetEdges() | (1 << direction_reverse(direction)));
            if (tileElement->AsPath()->IsQueue())
            {
                footpath_queue_chain_push(tileElement->AsPath()->GetRideIndex());
            }
        }
        if (!(flags & (GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)))
        {
            footpath_interrupt_peeps({ targetPos, tileElement->GetBaseZ() });
        }
        map_invalidate_element(targetPos, tileElement);
    }

loc_6A6FD2:
    if ((initialTileElement)->GetType() == TILE_ELEMENT_TYPE_PATH)
    {
        if (!query)
        {
            initialTileElement->AsPath()->SetEdges(initialTileElement->AsPath()->GetEdges() | (1 << direction));
            map_invalidate_element(initialTileElementPos, initialTileElement);
        }
    }
}

// TODO: Change this into a simple check that validates if the direction should be fully checked with loc_6A6D7E and move the
// calling of loc_6A6D7E into the parent function.
static void loc_6A6C85(
    const CoordsXYE& tileElementPos, int32_t direction, int32_t flags, bool query, rct_neighbour_list* neighbourList)
{
    if (query
        && fence_in_the_way(
            { tileElementPos, tileElementPos.element->GetBaseZ(), tileElementPos.element->GetClearanceZ() }, direction))
        return;

    if (tileElementPos.element->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
    {
        if (!entrance_has_direction(tileElementPos.element, direction - tileElementPos.element->GetDirection()))
        {
            return;
        }
    }

    if (tileElementPos.element->GetType() == TILE_ELEMENT_TYPE_TRACK)
    {
        auto ride = get_ride(tileElementPos.element->AsTrack()->GetRideIndex());
        if (ride == nullptr || !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            return;
        }
        const auto trackType = tileElementPos.element->AsTrack()->GetTrackType();
        const uint8_t trackSequence = tileElementPos.element->AsTrack()->GetSequenceIndex();
        if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH))
        {
            return;
        }
        uint16_t dx = (direction - tileElementPos.element->GetDirection()) & TILE_ELEMENT_DIRECTION_MASK;
        if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & (1 << dx)))
        {
            return;
        }
    }

    auto pos = CoordsXYZ{ tileElementPos, tileElementPos.element->GetBaseZ() };
    if (tileElementPos.element->GetType() == TILE_ELEMENT_TYPE_PATH)
    {
        if (tileElementPos.element->AsPath()->IsSloped())
        {
            if ((tileElementPos.element->AsPath()->GetSlopeDirection() - direction) & 1)
            {
                return;
            }
            if (tileElementPos.element->AsPath()->GetSlopeDirection() == direction)
            {
                pos.z += LAND_HEIGHT_STEP;
            }
        }
    }

    loc_6A6D7E(pos, direction, tileElementPos.element, flags, query, neighbourList);
}

/**
 *
 *  rct2: 0x006A6C66
 */
void footpath_connect_edges(const CoordsXY& footpathPos, TileElement* tileElement, int32_t flags)
{
    rct_neighbour_list neighbourList;
    rct_neighbour neighbour;

    footpath_update_queue_chains();

    neighbour_list_init(&neighbourList);

    footpath_update_queue_entrance_banner(footpathPos, tileElement);
    for (Direction direction : ALL_DIRECTIONS)
    {
        loc_6A6C85({ footpathPos, tileElement }, direction, flags, true, &neighbourList);
    }

    neighbour_list_sort(&neighbourList);

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH && tileElement->AsPath()->IsQueue())
    {
        ride_id_t rideIndex = RIDE_ID_NULL;
        uint8_t entranceIndex = 255;
        for (size_t i = 0; i < neighbourList.count; i++)
        {
            if (neighbourList.items[i].ride_index != RIDE_ID_NULL)
            {
                if (rideIndex == RIDE_ID_NULL)
                {
                    rideIndex = neighbourList.items[i].ride_index;
                    entranceIndex = neighbourList.items[i].entrance_index;
                }
                else if (rideIndex != neighbourList.items[i].ride_index)
                {
                    neighbour_list_remove(&neighbourList, i);
                }
                else if (
                    rideIndex == neighbourList.items[i].ride_index && entranceIndex != neighbourList.items[i].entrance_index
                    && neighbourList.items[i].entrance_index != 255)
                {
                    neighbour_list_remove(&neighbourList, i);
                }
            }
        }

        neighbourList.count = std::min<size_t>(neighbourList.count, 2);
    }

    while (neighbour_list_pop(&neighbourList, &neighbour))
    {
        loc_6A6C85({ footpathPos, tileElement }, neighbour.direction, flags, false, nullptr);
    }

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
    {
        footpath_connect_corners(footpathPos, tileElement->AsPath());
    }
}

/**
 *
 *  rct2: 0x006A742F
 */
void footpath_chain_ride_queue(
    ride_id_t rideIndex, int32_t entranceIndex, const CoordsXY& initialFootpathPos, TileElement* const initialTileElement,
    int32_t direction)
{
    TileElement *lastPathElement, *lastQueuePathElement;
    auto tileElement = initialTileElement;
    auto curQueuePos = initialFootpathPos;
    auto lastPath = curQueuePos;
    int32_t baseZ = tileElement->GetBaseZ();
    int32_t lastPathDirection = direction;

    lastPathElement = nullptr;
    lastQueuePathElement = nullptr;
    for (;;)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            lastPathElement = tileElement;
            lastPath = curQueuePos;
            lastPathDirection = direction;
            if (tileElement->AsPath()->IsSloped())
            {
                if (tileElement->AsPath()->GetSlopeDirection() == direction)
                {
                    baseZ += LAND_HEIGHT_STEP;
                }
            }
        }

        auto targetQueuePos = curQueuePos + CoordsDirectionDelta[direction];
        tileElement = map_get_first_element_at(targetQueuePos);
        if (tileElement != nullptr)
        {
            do
            {
                if (lastQueuePathElement == tileElement)
                    continue;
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
                    continue;
                if (tileElement->GetBaseZ() == baseZ)
                {
                    if (tileElement->AsPath()->IsSloped())
                    {
                        if (tileElement->AsPath()->GetSlopeDirection() != direction)
                            break;
                    }
                    goto foundNextPath;
                }
                if (tileElement->GetBaseZ() == baseZ - LAND_HEIGHT_STEP)
                {
                    if (!tileElement->AsPath()->IsSloped())
                        break;

                    if (direction_reverse(tileElement->AsPath()->GetSlopeDirection()) != direction)
                        break;

                    baseZ -= LAND_HEIGHT_STEP;
                    goto foundNextPath;
                }
            } while (!(tileElement++)->IsLastForTile());
        }
        break;

    foundNextPath:
        if (tileElement->AsPath()->IsQueue())
        {
            // Fix #2051: Stop queue paths that are already connected to two other tiles
            //            from connecting to the tile we are coming from.
            int32_t edges = tileElement->AsPath()->GetEdges();
            int32_t numEdges = bitcount(edges);
            if (numEdges >= 2)
            {
                int32_t requiredEdgeMask = 1 << direction_reverse(direction);
                if (!(edges & requiredEdgeMask))
                {
                    break;
                }
            }

            tileElement->AsPath()->SetHasQueueBanner(false);
            tileElement->AsPath()->SetEdges(tileElement->AsPath()->GetEdges() | (1 << direction_reverse(direction)));
            tileElement->AsPath()->SetRideIndex(rideIndex);
            tileElement->AsPath()->SetStationIndex(entranceIndex);

            curQueuePos = targetQueuePos;
            map_invalidate_element(targetQueuePos, tileElement);

            if (lastQueuePathElement == nullptr)
            {
                lastQueuePathElement = tileElement;
            }

            if (tileElement->AsPath()->GetEdges() & (1 << direction))
                continue;

            direction = (direction + 1) & 3;
            if (tileElement->AsPath()->GetEdges() & (1 << direction))
                continue;

            direction = direction_reverse(direction);
            if (tileElement->AsPath()->GetEdges() & (1 << direction))
                continue;
        }
        break;
    }

    if (rideIndex != RIDE_ID_NULL && lastPathElement != nullptr)
    {
        if (lastPathElement->AsPath()->IsQueue())
        {
            lastPathElement->AsPath()->SetHasQueueBanner(true);
            lastPathElement->AsPath()->SetQueueBannerDirection(lastPathDirection); // set the ride sign direction

            map_animation_create(MAP_ANIMATION_TYPE_QUEUE_BANNER, { lastPath, lastPathElement->GetBaseZ() });
        }
    }
}

void footpath_queue_chain_reset()
{
    _footpathQueueChainNext = _footpathQueueChain;
}

/**
 *
 *  rct2: 0x006A76E9
 */
void footpath_queue_chain_push(ride_id_t rideIndex)
{
    if (rideIndex != RIDE_ID_NULL)
    {
        uint8_t* lastSlot = _footpathQueueChain + std::size(_footpathQueueChain) - 1;
        if (_footpathQueueChainNext <= lastSlot)
        {
            *_footpathQueueChainNext++ = rideIndex;
        }
    }
}

/**
 *
 *  rct2: 0x006A759F
 */
void footpath_update_queue_chains()
{
    for (uint8_t* queueChainPtr = _footpathQueueChain; queueChainPtr < _footpathQueueChainNext; queueChainPtr++)
    {
        ride_id_t rideIndex = *queueChainPtr;
        auto ride = get_ride(rideIndex);
        if (ride == nullptr)
            continue;

        for (int32_t i = 0; i < MAX_STATIONS; i++)
        {
            TileCoordsXYZD location = ride_get_entrance_location(ride, i);
            if (location.isNull())
                continue;

            TileElement* tileElement = map_get_first_element_at(location.ToCoordsXY());
            if (tileElement != nullptr)
            {
                do
                {
                    if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                        continue;
                    if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
                        continue;
                    if (tileElement->AsEntrance()->GetRideIndex() != rideIndex)
                        continue;

                    Direction direction = direction_reverse(tileElement->GetDirection());
                    footpath_chain_ride_queue(rideIndex, i, location.ToCoordsXY(), tileElement, direction);
                } while (!(tileElement++)->IsLastForTile());
            }
        }
    }
}

/**
 *
 *  rct2: 0x0069ADBD
 */
static void footpath_fix_ownership(const CoordsXY& mapPos)
{
    const auto* surfaceElement = map_get_surface_element_at(mapPos);
    uint16_t ownership;

    // Unlikely to be NULL unless deliberate.
    if (surfaceElement != nullptr)
    {
        // If the tile is not safe to own construction rights of, erase them.
        if (check_max_allowable_land_rights_for_tile({ mapPos, surfaceElement->base_height << 3 }) == OWNERSHIP_UNOWNED)
        {
            ownership = OWNERSHIP_UNOWNED;
        }
        // If the tile is safe to own construction rights of, do not erase contruction rights.
        else
        {
            ownership = surfaceElement->GetOwnership();
            // You can't own the entrance path.
            if (ownership == OWNERSHIP_OWNED || ownership == OWNERSHIP_AVAILABLE)
            {
                ownership = OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED;
            }
        }
    }
    else
    {
        ownership = OWNERSHIP_UNOWNED;
    }

    auto landSetRightsAction = LandSetRightsAction(mapPos, LandSetRightSetting::SetOwnershipWithChecks, ownership);
    landSetRightsAction.SetFlags(GAME_COMMAND_FLAG_NO_SPEND);
    GameActions::Execute(&landSetRightsAction);
}

static bool get_next_direction(int32_t edges, int32_t* direction)
{
    int32_t index = bitscanforward(edges);
    if (index == -1)
        return false;

    *direction = index;
    return true;
}

/**
 *
 *  rct2: 0x0069AC1A
 * @param flags (1 << 0): Ignore queues
 *              (1 << 5): Unown
 *              (1 << 7): Ignore no entry signs
 */
static int32_t footpath_is_connected_to_map_edge_recurse(
    const CoordsXYZ& footpathPos, int32_t direction, int32_t flags, int32_t level, int32_t distanceFromJunction,
    int32_t junctionTolerance)
{
    TileElement* tileElement;
    int32_t edges, slopeDirection;

    auto targetPos = CoordsXYZ{ CoordsXY{ footpathPos } + CoordsDirectionDelta[direction], footpathPos.z };
    if (++level > 250)
        return FOOTPATH_SEARCH_TOO_COMPLEX;

    // Check if we are at edge of map
    if (targetPos.x < COORDS_XY_STEP || targetPos.y < COORDS_XY_STEP)
        return FOOTPATH_SEARCH_SUCCESS;
    if (targetPos.x >= gMapSizeUnits || targetPos.y >= gMapSizeUnits)
        return FOOTPATH_SEARCH_SUCCESS;

    tileElement = map_get_first_element_at(targetPos);
    if (tileElement == nullptr)
        return level == 1 ? FOOTPATH_SEARCH_NOT_FOUND : FOOTPATH_SEARCH_INCOMPLETE;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tileElement->AsPath()->IsSloped() && (slopeDirection = tileElement->AsPath()->GetSlopeDirection()) != direction)
        {
            if (direction_reverse(slopeDirection) != direction)
                continue;
            if (tileElement->GetBaseZ() + PATH_HEIGHT_STEP != targetPos.z)
                continue;
        }
        else if (tileElement->GetBaseZ() != targetPos.z)
        {
            continue;
        }

        if (!(flags & (1 << 0)))
        {
            if (tileElement->AsPath()->IsQueue())
            {
                continue;
            }
        }

        if (flags & (1 << 5))
        {
            footpath_fix_ownership(targetPos);
        }
        edges = tileElement->AsPath()->GetEdges();
        direction = direction_reverse(direction);
        if (!(flags & (1 << 7)))
        {
            if (tileElement[1].GetType() == TILE_ELEMENT_TYPE_BANNER)
            {
                for (int32_t i = 1; i < 4; i++)
                {
                    if ((&tileElement[i - 1])->IsLastForTile())
                        break;
                    if (tileElement[i].GetType() != TILE_ELEMENT_TYPE_BANNER)
                        break;
                    edges &= tileElement[i].AsBanner()->GetAllowedEdges();
                }
            }
            if (tileElement[2].GetType() == TILE_ELEMENT_TYPE_BANNER && tileElement[1].GetType() != TILE_ELEMENT_TYPE_PATH)
            {
                for (int32_t i = 1; i < 6; i++)
                {
                    if ((&tileElement[i - 1])->IsLastForTile())
                        break;
                    if (tileElement[i].GetType() != TILE_ELEMENT_TYPE_BANNER)
                        break;
                    edges &= tileElement[i].AsBanner()->GetAllowedEdges();
                }
            }
        }
        goto searchFromFootpath;
    } while (!(tileElement++)->IsLastForTile());
    return level == 1 ? FOOTPATH_SEARCH_NOT_FOUND : FOOTPATH_SEARCH_INCOMPLETE;

searchFromFootpath:
    // Exclude direction we came from
    targetPos.z = tileElement->GetBaseZ();
    edges &= ~(1 << direction);

    // Find next direction to go
    int32_t newDirection{};
    if (!get_next_direction(edges, &newDirection))
    {
        return FOOTPATH_SEARCH_INCOMPLETE;
    }
    direction = newDirection;

    edges &= ~(1 << direction);
    if (edges == 0)
    {
        // Only possible direction to go
        if (tileElement->AsPath()->IsSloped() && tileElement->AsPath()->GetSlopeDirection() == direction)
        {
            targetPos.z += PATH_HEIGHT_STEP;
        }
        return footpath_is_connected_to_map_edge_recurse(
            targetPos, direction, flags, level, distanceFromJunction + 1, junctionTolerance);
    }
    else
    {
        // We have reached a junction
        if (distanceFromJunction != 0)
        {
            junctionTolerance--;
        }
        junctionTolerance--;
        if (junctionTolerance < 0)
        {
            return FOOTPATH_SEARCH_TOO_COMPLEX;
        }

        do
        {
            direction = newDirection;
            edges &= ~(1 << direction);
            if (tileElement->AsPath()->IsSloped() && tileElement->AsPath()->GetSlopeDirection() == direction)
            {
                targetPos.z += PATH_HEIGHT_STEP;
            }
            int32_t result = footpath_is_connected_to_map_edge_recurse(
                targetPos, direction, flags, level, 0, junctionTolerance);
            if (result == FOOTPATH_SEARCH_SUCCESS)
            {
                return result;
            }
        } while (get_next_direction(edges, &newDirection));

        return FOOTPATH_SEARCH_INCOMPLETE;
    }
}

// TODO: Use GAME_COMMAND_FLAGS
int32_t footpath_is_connected_to_map_edge(const CoordsXYZ& footpathPos, int32_t direction, int32_t flags)
{
    flags |= (1 << 0);
    return footpath_is_connected_to_map_edge_recurse(footpathPos, direction, flags, 0, 0, 16);
}

bool PathElement::IsSloped() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED) != 0;
}

void PathElement::SetSloped(bool isSloped)
{
    Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED;
    if (isSloped)
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED;
}

Direction PathElement::GetSlopeDirection() const
{
    return SlopeDirection;
}

void PathElement::SetSlopeDirection(Direction newSlope)
{
    SlopeDirection = newSlope;
}

bool PathElement::IsQueue() const
{
    return (type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE) != 0;
}

void PathElement::SetIsQueue(bool isQueue)
{
    type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
    if (isQueue)
        type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
}

bool PathElement::HasQueueBanner() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER) != 0;
}

void PathElement::SetHasQueueBanner(bool hasQueueBanner)
{
    Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER;
    if (hasQueueBanner)
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER;
}

bool PathElement::IsBroken() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN) != 0;
}

void PathElement::SetIsBroken(bool isBroken)
{
    if (isBroken)
    {
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN;
    }
    else
    {
        Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN;
    }
}

bool PathElement::IsBlockedByVehicle() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE) != 0;
}

void PathElement::SetIsBlockedByVehicle(bool isBlocked)
{
    if (isBlocked)
    {
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE;
    }
    else
    {
        Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE;
    }
}

::StationIndex PathElement::GetStationIndex() const
{
    return StationIndex;
}

void PathElement::SetStationIndex(::StationIndex newStationIndex)
{
    StationIndex = newStationIndex;
}

bool PathElement::IsWide() const
{
    return (type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE) != 0;
}

void PathElement::SetWide(bool isWide)
{
    type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
    if (isWide)
        type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
}

bool PathElement::IsWideForGroup(uint8_t wideGroup) const
{
    return (WideFlags & (1 << wideGroup)) != 0;
}

uint8_t PathElement::GetWideFlags() const
{
    return WideFlags;
}

void PathElement::SetWideForGroup(uint8_t wideGroup, bool isWide)
{
    WideFlags &= ~(1 << wideGroup);
    if (isWide)
        WideFlags |= (1 << wideGroup);
}

void PathElement::SetWideFlags(uint8_t flags)
{
    WideFlags = flags;
}

bool PathElement::HasAddition() const
{
    return Additions != 0;
}

uint8_t PathElement::GetAddition() const
{
    return Additions;
}

uint8_t PathElement::GetAdditionEntryIndex() const
{
    return GetAddition() - 1;
}

rct_scenery_entry* PathElement::GetAdditionEntry() const
{
    return get_footpath_item_entry(GetAdditionEntryIndex());
}

void PathElement::SetAddition(uint8_t newAddition)
{
    Additions = newAddition;
}

bool PathElement::AdditionIsGhost() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST) != 0;
}

void PathElement::SetAdditionIsGhost(bool isGhost)
{
    Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST;
    if (isGhost)
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST;
}

PathSurfaceIndex PathElement::GetSurfaceEntryIndex() const
{
    return SurfaceIndex;
}

PathRailingsIndex PathElement::GetRailingEntryIndex() const
{
    return GetSurfaceEntryIndex();
}

PathSurfaceEntry* PathElement::GetSurfaceEntry() const
{
    if (!IsQueue())
        return get_path_surface_entry(GetSurfaceEntryIndex());
    else
        return get_path_surface_entry(GetSurfaceEntryIndex() + MAX_PATH_OBJECTS);
}

PathRailingsEntry* PathElement::GetRailingEntry() const
{
    return get_path_railings_entry(GetRailingEntryIndex());
}

void PathElement::SetSurfaceEntryIndex(PathSurfaceIndex newIndex)
{
    SurfaceIndex = newIndex & ~FOOTPATH_ELEMENT_INSERT_QUEUE;
}

void PathElement::SetRailingEntryIndex(PathRailingsIndex newEntryIndex)
{
    log_verbose("Setting railing entry index to %d", newEntryIndex);
}

uint8_t PathElement::GetQueueBannerDirection() const
{
    return ((type & FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK) >> 6);
}

void PathElement::SetQueueBannerDirection(uint8_t direction)
{
    type &= ~FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK;
    type |= (direction << 6);
}

bool PathElement::ShouldDrawPathOverSupports()
{
    return (GetRailingEntry()->flags & RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS);
}

void PathElement::SetShouldDrawPathOverSupports(bool on)
{
    log_verbose("Setting 'draw path over supports' to %d", (size_t)on);
}

/**
 *
 *  rct2: 0x006A8B12
 *  clears the wide footpath flag for all footpaths
 *  at location
 */
static void footpath_clear_wide(const CoordsXY& footpathPos, uint8_t wideGroup)
{
    TileElement* tileElement = map_get_first_element_at(footpathPos);
    auto extPathVector = map_get_extended_data_vector_at(footpathPos);

    if (tileElement == nullptr)
    {
        for (int i = 0; i < extPathVector->size(); i++)
            delete extPathVector->at(i);

        extPathVector->clear();
        return;
    }
    int i = 0;
    do
    {
        if (extPathVector->size() <= i)
        {
            extPathVector->push_back(new ExtendedPathData());
        }

        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        tileElement->AsPath()->SetWideForGroup(wideGroup, false);
        extPathVector->at(i)->SetWideForGroup(wideGroup, false);

        if (wideGroup == WIDE_GROUP_N_SW)
            tileElement->AsPath()->SetWide(false);

    } while (!(tileElement++)->IsLastForTile() && i++);
}

/**
 *
 *  rct2: 0x006A8ACF
 *  returns footpath element if it can be made wide
 *  returns NULL if it can not be made wide
 */
static TileElement* footpath_can_be_wide(const CoordsXYZ& footpathPos)
{
    TileElement* tileElement = map_get_first_element_at(footpathPos);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (footpathPos.z != tileElement->GetBaseZ())
            continue;
        if (tileElement->AsPath()->IsQueue())
            continue;
        if (tileElement->AsPath()->IsSloped())
            continue;
        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

static ExtendedPathData* extended_footpath_can_be_wide(const CoordsXYZ& footpathPos, uint8_t wideLevel)
{
    TileElement* tileElement = map_get_first_element_at(footpathPos);
    auto extData = map_get_extended_data_vector_at(footpathPos);
    if (tileElement == nullptr)
    {
        return nullptr;
    }

    int i = 0;
    do
    {
        if (extData->size() <= i)
            extData->push_back(new ExtendedPathData(0));

        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (footpathPos.z != tileElement->GetBaseZ())
            continue;
        if (tileElement->AsPath()->IsQueue())
            continue;
        if (tileElement->AsPath()->IsSloped())
            continue;

        return extData->at(i);
    } while (!(tileElement++)->IsLastForTile() && i++);

    return nullptr;
}

/**
 *
 *  rct2: 0x006A87BB
 */
void footpath_update_path_wide_flags(const CoordsXY& footpathPos, uint8_t wideGroup)
{
    if (map_is_location_at_edge(footpathPos))
        return;

    footpath_clear_wide(footpathPos, wideGroup);
    /* Rather than clearing the wide flag of the following tiles and
     * checking the state of them later, leave them intact and assume
     * they were cleared. Consequently only the wide flag for this single
     * tile is modified by this update.
     * This is important for avoiding glitches in pathfinding that occurs
     * between the batches of updates to the path wide flags.
     * Corresponding pathList[] indexes for the following tiles
     * are: 2, 3, 4, 5.
     * Note: indexes 3, 4, 5 are reset in the current call;
     *       index 2 is reset in the previous call. */
    // x += 0x20;
    // footpath_clear_wide(x, y);
    // y += 0x20;
    // footpath_clear_wide(x, y);
    // x -= 0x20;
    // footpath_clear_wide(x, y);
    // y -= 0x20;

    TileElement* tileElement = map_get_first_element_at(footpathPos);
    auto extPathVector = map_get_extended_data_vector_at(footpathPos);

    if (tileElement == nullptr)
        return;

    int i = 0;
    do
    {
        if (extPathVector->size() <= 0)
            extPathVector->push_back(new ExtendedPathData);

        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tileElement->AsPath()->IsQueue())
            continue;

        if (tileElement->AsPath()->IsSloped())
            continue;

        if (tileElement->AsPath()->GetEdges() == 0)
            continue;

        auto height = tileElement->GetBaseZ();

        // pathList is a list of elements, set by sub_6A8ACF adjacent to x,y
        // Spanned from 0x00F3EFA8 to 0x00F3EFC7 (8 elements) in the original
        std::array<TileElement*, 8> pathList;
        std::array<ExtendedPathData*, 8> extendedPathList;
        for (int32_t direction = 0; direction < 8; ++direction)
        {
            auto footpathLoc = CoordsXYZ(footpathPos + CoordsDirectionDelta[direction], height);
            pathList[direction] = footpath_can_be_wide(footpathLoc);
            extendedPathList[direction] = extended_footpath_can_be_wide(footpathLoc, 0);
        }

        uint8_t pathConnections = 0;

        const uint8_t* wideGroupDirections = AllWideGroupDirections[wideGroup];
        const uint8_t* wideGroupConnections = AllWideGroupConnections[wideGroup];
        const uint8_t* wideGroupEdges = AllWideGroupEdges[wideGroup];

        if (tileElement->AsPath()->GetEdges() & wideGroupEdges[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE])
        {
            pathConnections |= wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE];
            if (pathList[wideGroupDirections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE]] != nullptr
                && pathList[wideGroupDirections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE]]->AsPath()->IsWideForGroup(wideGroup))
            {
                pathConnections &= ~wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE];
            }
        }

        if (tileElement->AsPath()->GetEdges() & wideGroupEdges[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE])
        {
            pathConnections |= wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE];
            if (pathList[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE]] != nullptr
                && pathList[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE]]->AsPath()->IsWideForGroup(wideGroup))
            {
                pathConnections &= ~wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE];
            }
        }

        if (tileElement->AsPath()->GetEdges() & wideGroupEdges[WIDE_GROUP_SECONDARY_DIRECTION])
        {
            pathConnections |= wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION];
            /* In the following:
             * footpath_element_is_wide(pathlist[wideGroupDirections[WIDE_GROUP_SECONDARY_DIRECTION]])
             * is always false due to the tile update order
             * in combination with reset tiles.
             * Commented out since it will never occur. */
            // if (pathlist[wideGroupDirections[WIDE_GROUP_SECONDARY_DIRECTION]] != nullptr) {
            //  if (footpath_element_is_wide(pathlist[wideGroupDirections[WIDE_GROUP_SECONDARY_DIRECTION]])) {
            //      pathConnections &= ~wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION];
            //  }
            //}
        }

        if (tileElement->AsPath()->GetEdges() & wideGroupEdges[WIDE_GROUP_PRIMARY_DIRECTION])
        {
            pathConnections |= wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION];
            /* In the following:
             * footpath_element_is_wide(pathlist[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION]])
             * is always false due to the tile update order
             * in combination with reset tiles.
             * Commented out since it will never occur. */
            // if (pathlist[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION]] != nullptr) {
            //  if (footpath_element_is_wide(pathlist[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION]])) {
            //      pathConnections &= ~wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION];
            //  }
            //}
        }

        if ((pathConnections & wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE])
            && pathList[wideGroupDirections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE]] != nullptr
            && !pathList[wideGroupDirections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE]]->AsPath()->IsWideForGroup(wideGroup))
        {
            uint8_t edgeMask1 = wideGroupEdges[WIDE_GROUP_SECONDARY_DIRECTION] | wideGroupEdges[WIDE_GROUP_PRIMARY_DIRECTION];
            if ((pathConnections & wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE])
                && pathList[wideGroupDirections[WIDE_GROUP_FIRST_CORNER]] != nullptr
                && !pathList[wideGroupDirections[WIDE_GROUP_FIRST_CORNER]]->AsPath()->IsWideForGroup(wideGroup)
                && (pathList[wideGroupDirections[WIDE_GROUP_FIRST_CORNER]]->AsPath()->GetEdges() & edgeMask1) == edgeMask1
                && pathList[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE]] != nullptr
                && !pathList[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE]]->AsPath()->IsWideForGroup(wideGroup))
            {
                pathConnections |= wideGroupConnections[WIDE_GROUP_FINAL_CORNER_CARDINAL];
            }

            /* In the following:
             * footpath_element_is_wide(pathList[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION_0]])
             * is always false due to the tile update order
             * in combination with reset tiles.
             * Short circuit the logic appropriately. */
            uint8_t edgeMask2 = wideGroupEdges[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE]
                | wideGroupEdges[WIDE_GROUP_SECONDARY_DIRECTION];
            if ((pathConnections & wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION])
                && pathList[wideGroupDirections[WIDE_GROUP_SECOND_CORNER]] != nullptr
                && !(pathList[wideGroupDirections[WIDE_GROUP_SECOND_CORNER]])->AsPath()->IsWideForGroup(wideGroup)
                && (pathList[wideGroupDirections[WIDE_GROUP_SECOND_CORNER]]->AsPath()->GetEdges() & edgeMask2) == edgeMask2
                && pathList[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION]] != nullptr)
            {
                pathConnections |= wideGroupConnections[WIDE_GROUP_THIRD_CORNER_CARDINAL];
            }
        }

        /* In the following:
         * footpath_element_is_wide(wideGroupDirections[WIDE_GROUP_THIRD_CORNER_CARDINAL])
         * footpath_element_is_wide(wideGroupDirections[WIDE_GROUP_SECONDARY_DIRECTION])
         * are always false due to the tile update order
         * in combination with reset tiles.
         * Short circuit the logic appropriately. */
        if ((pathConnections & wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION])
            && pathList[wideGroupDirections[WIDE_GROUP_SECONDARY_DIRECTION]] != nullptr)
        {
            uint8_t edgeMask1 = wideGroupEdges[WIDE_GROUP_PRIMARY_DIRECTION]
                | wideGroupEdges[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE];
            if ((pathConnections & wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE])
                && (pathList[wideGroupDirections[WIDE_GROUP_THIRD_CORNER_CARDINAL]] != nullptr)
                && (pathList[wideGroupDirections[WIDE_GROUP_THIRD_CORNER_CARDINAL]]->AsPath()->GetEdges() & edgeMask1)
                    == edgeMask1
                && pathList[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE]] != nullptr
                && !pathList[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE]]->AsPath()->IsWideForGroup(wideGroup))
            {
                pathConnections |= wideGroupConnections[WIDE_GROUP_SECOND_CORNER]; // xx
            }

            /* In the following:
             * footpath_element_is_wide(wideGroupDirections[WIDE_GROUP_FINAL_CORNER_CARDINAL])
             * footpath_element_is_wide(wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION])
             * are always false due to the tile update order
             * in combination with reset tiles.
             * Short circuit the logic appropriately. */
            uint8_t edgeMask2 = wideGroupEdges[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE]
                | wideGroupEdges[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE];
            if ((pathConnections & wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION])
                && pathList[wideGroupDirections[WIDE_GROUP_FINAL_CORNER_CARDINAL]] != nullptr
                && (pathList[wideGroupDirections[WIDE_GROUP_FINAL_CORNER_CARDINAL]]->AsPath()->GetEdges() & edgeMask2)
                    == edgeMask2
                && pathList[wideGroupDirections[WIDE_GROUP_PRIMARY_DIRECTION]] != nullptr)
            {
                pathConnections |= wideGroupConnections[WIDE_GROUP_FIRST_CORNER];
            }
        }

        if ((pathConnections & wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE])
            && (pathConnections
                & (wideGroupConnections[WIDE_GROUP_THIRD_CORNER_CARDINAL]
                   | wideGroupConnections[WIDE_GROUP_FINAL_CORNER_CARDINAL])))
        {
            pathConnections &= ~wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE];
        }

        if ((pathConnections & wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE])
            && (pathConnections
                & (wideGroupConnections[WIDE_GROUP_SECOND_CORNER] | wideGroupConnections[WIDE_GROUP_FINAL_CORNER_CARDINAL])))
        {
            pathConnections &= ~wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE];
        }

        if ((pathConnections & wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION])
            && (pathConnections
                & (wideGroupConnections[WIDE_GROUP_FIRST_CORNER] | wideGroupConnections[WIDE_GROUP_SECOND_CORNER])))
        {
            pathConnections &= ~wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION];
        }

        if ((pathConnections & wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION])
            && (pathConnections
                & (wideGroupConnections[WIDE_GROUP_THIRD_CORNER_CARDINAL] | wideGroupConnections[WIDE_GROUP_FIRST_CORNER])))
        {
            pathConnections &= ~wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION];
        }

        if (!(pathConnections
              & (wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION_REVERSE]
                 | wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION] | wideGroupConnections[WIDE_GROUP_PRIMARY_DIRECTION]
                 | wideGroupConnections[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE])))
        {
            uint8_t e = tileElement->AsPath()->GetEdgesAndCorners();
            if ((e != 0b10101111) && (e != 0b01011111) && (e != ~(wideGroupEdges[WIDE_GROUP_SECONDARY_DIRECTION_REVERSE] << 4)))
            {
                tileElement->AsPath()->SetWideForGroup(wideGroup, true);
                extPathVector->at(i)->SetWideForGroup(wideGroup, true);
            }

            if (wideGroup == 0 && tileElement->AsPath()->IsWideForGroup(0))
                tileElement->AsPath()->SetWide(true);
        }
    } while (!(tileElement++)->IsLastForTile() && i++);
}

bool footpath_is_blocked_by_vehicle(const TileCoordsXYZ& position)
{
    auto pathElement = map_get_path_element_at(position);
    return pathElement != nullptr && pathElement->IsBlockedByVehicle();
}

/**
 *
 *  rct2: 0x006A7642
 */
void footpath_update_queue_entrance_banner(const CoordsXY& footpathPos, TileElement* tileElement)
{
    int32_t elementType = tileElement->GetType();
    switch (elementType)
    {
        case TILE_ELEMENT_TYPE_PATH:
            if (tileElement->AsPath()->IsQueue())
            {
                footpath_queue_chain_push(tileElement->AsPath()->GetRideIndex());
                for (int32_t direction = 0; direction < 4; direction++)
                {
                    if (tileElement->AsPath()->GetEdges() & (1 << direction))
                    {
                        footpath_chain_ride_queue(255, 0, footpathPos, tileElement, direction);
                    }
                }
                tileElement->AsPath()->SetRideIndex(RIDE_ID_NULL);
            }
            break;
        case TILE_ELEMENT_TYPE_ENTRANCE:
            if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
            {
                footpath_queue_chain_push(tileElement->AsEntrance()->GetRideIndex());
                footpath_chain_ride_queue(255, 0, footpathPos, tileElement, direction_reverse(tileElement->GetDirection()));
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006A6B7F
 */
static void footpath_remove_edges_towards_here(
    const CoordsXYZ& footpathPos, int32_t direction, TileElement* tileElement, bool isQueue)
{
    if (tileElement->AsPath()->IsQueue())
    {
        footpath_queue_chain_push(tileElement->AsPath()->GetRideIndex());
    }

    auto d = direction_reverse(direction);
    tileElement->AsPath()->SetEdges(tileElement->AsPath()->GetEdges() & ~(1 << d));
    int32_t cd = ((d - 1) & 3);
    tileElement->AsPath()->SetCorners(tileElement->AsPath()->GetCorners() & ~(1 << cd));
    cd = ((cd + 1) & 3);
    tileElement->AsPath()->SetCorners(tileElement->AsPath()->GetCorners() & ~(1 << cd));
    map_invalidate_tile({ footpathPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });

    if (isQueue)
        footpath_disconnect_queue_from_path(footpathPos, tileElement, -1);

    Direction shiftedDirection = (direction + 1) & 3;
    auto targetFootPathPos = CoordsXYZ{ CoordsXY{ footpathPos } + CoordsDirectionDelta[shiftedDirection], footpathPos.z };

    tileElement = map_get_first_element_at(targetFootPathPos);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (tileElement->GetBaseZ() != targetFootPathPos.z)
            continue;

        if (tileElement->AsPath()->IsSloped())
            break;

        cd = ((shiftedDirection + 1) & 3);
        tileElement->AsPath()->SetCorners(tileElement->AsPath()->GetCorners() & ~(1 << cd));
        map_invalidate_tile({ targetFootPathPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
        break;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006A6B14
 */
static void footpath_remove_edges_towards(const CoordsXYRangedZ& footPathPos, int32_t direction, bool isQueue)
{
    if (!map_is_location_valid(footPathPos))
    {
        return;
    }

    TileElement* tileElement = map_get_first_element_at(footPathPos);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (footPathPos.clearanceZ == tileElement->GetBaseZ())
        {
            if (tileElement->AsPath()->IsSloped())
            {
                uint8_t slope = tileElement->AsPath()->GetSlopeDirection();
                if (slope != direction)
                    break;
            }
            footpath_remove_edges_towards_here({ footPathPos, footPathPos.clearanceZ }, direction, tileElement, isQueue);
            break;
        }

        if (footPathPos.baseZ == tileElement->GetBaseZ())
        {
            if (!tileElement->AsPath()->IsSloped())
                break;

            uint8_t slope = direction_reverse(tileElement->AsPath()->GetSlopeDirection());
            if (slope != direction)
                break;

            footpath_remove_edges_towards_here({ footPathPos, footPathPos.clearanceZ }, direction, tileElement, isQueue);
            break;
        }
    } while (!(tileElement++)->IsLastForTile());
}

// Returns true when there is an element at the given coordinates that want to connect to a path with the given direction (ride
// entrances and exits, shops, paths).
bool tile_element_wants_path_connection_towards(const TileCoordsXYZD& coords, const TileElement* const elementToBeRemoved)
{
    TileElement* tileElement = map_get_first_element_at(coords.ToCoordsXY());
    if (tileElement == nullptr)
        return false;
    do
    {
        // Don't check the element that gets removed
        if (tileElement == elementToBeRemoved)
            continue;

        switch (tileElement->GetType())
        {
            case TILE_ELEMENT_TYPE_PATH:
                if (tileElement->base_height == coords.z)
                {
                    if (!tileElement->AsPath()->IsSloped())
                        // The footpath is flat, it can be connected to from any direction
                        return true;
                    else if (tileElement->AsPath()->GetSlopeDirection() == direction_reverse(coords.direction))
                        // The footpath is sloped and its lowest point matches the edge connection
                        return true;
                }
                else if (tileElement->base_height + 2 == coords.z)
                {
                    if (tileElement->AsPath()->IsSloped() && tileElement->AsPath()->GetSlopeDirection() == coords.direction)
                        // The footpath is sloped and its higher point matches the edge connection
                        return true;
                }
                break;
            case TILE_ELEMENT_TYPE_TRACK:
                if (tileElement->base_height == coords.z)
                {
                    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
                    if (ride == nullptr)
                        continue;

                    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                        break;

                    const auto trackType = tileElement->AsTrack()->GetTrackType();
                    const uint8_t trackSequence = tileElement->AsTrack()->GetSequenceIndex();
                    if (FlatRideTrackSequenceProperties[trackType][trackSequence] & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH)
                    {
                        uint16_t dx = ((coords.direction - tileElement->GetDirection()) & TILE_ELEMENT_DIRECTION_MASK);
                        if (FlatRideTrackSequenceProperties[trackType][trackSequence] & (1 << dx))
                        {
                            // Track element has the flags required for the given direction
                            return true;
                        }
                    }
                }
                break;
            case TILE_ELEMENT_TYPE_ENTRANCE:
                if (tileElement->base_height == coords.z)
                {
                    if (entrance_has_direction(tileElement, coords.direction - tileElement->GetDirection()))
                    {
                        // Entrance wants to be connected towards the given direction
                        return true;
                    }
                }
                break;
            default:
                break;
        }
    } while (!(tileElement++)->IsLastForTile());

    return false;
}

// fix up the corners around the given path element that gets removed
static void footpath_fix_corners_around(const TileCoordsXY& footpathPos, TileElement* pathElement)
{
    // A mask for the paths' corners of each possible neighbour
    static constexpr uint8_t cornersTouchingTile[3][3] = {
        { 0b0010, 0b0011, 0b0001 },
        { 0b0110, 0b0000, 0b1001 },
        { 0b0100, 0b1100, 0b1000 },
    };

    // Sloped paths don't create filled corners, so no need to remove any
    if (pathElement->GetType() == TILE_ELEMENT_TYPE_PATH && pathElement->AsPath()->IsSloped())
        return;

    for (int32_t xOffset = -1; xOffset <= 1; xOffset++)
    {
        for (int32_t yOffset = -1; yOffset <= 1; yOffset++)
        {
            // Skip self
            if (xOffset == 0 && yOffset == 0)
                continue;

            TileElement* tileElement = map_get_first_element_at(
                TileCoordsXY{ footpathPos.x + xOffset, footpathPos.y + yOffset }.ToCoordsXY());
            if (tileElement == nullptr)
                continue;
            do
            {
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
                    continue;
                if (tileElement->AsPath()->IsSloped())
                    continue;
                if (tileElement->base_height != pathElement->base_height)
                    continue;

                const int32_t ix = xOffset + 1;
                const int32_t iy = yOffset + 1;
                tileElement->AsPath()->SetCorners(tileElement->AsPath()->GetCorners() & ~(cornersTouchingTile[iy][ix]));
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}

/**
 *
 *  rct2: 0x006A6AA7
 * @param x x-coordinate in units (not tiles)
 * @param y y-coordinate in units (not tiles)
 */
void footpath_remove_edges_at(const CoordsXY& footpathPos, TileElement* tileElement)
{
    if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
    {
        auto rideIndex = tileElement->AsTrack()->GetRideIndex();
        auto ride = get_ride(rideIndex);
        if (ride == nullptr || !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            return;
    }

    footpath_update_queue_entrance_banner(footpathPos, tileElement);

    bool fixCorners = false;
    for (uint8_t direction = 0; direction < 4; direction++)
    {
        int32_t z1 = tileElement->base_height;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (tileElement->AsPath()->IsSloped())
            {
                int32_t slope = tileElement->AsPath()->GetSlopeDirection();
                // Sloped footpaths don't connect sideways
                if ((slope - direction) & 1)
                    continue;

                // When a path is sloped, the higher point of the path is 2 units higher
                z1 += (slope == direction) ? 2 : 0;
            }
        }

        // When clearance checks were disabled a neighbouring path can be connected to both the path-ghost and to something
        // else, so before removing edges from neighbouring paths we have to make sure there is nothing else they are connected
        // to.
        if (!tile_element_wants_path_connection_towards({ TileCoordsXY{ footpathPos }, z1, direction }, tileElement))
        {
            bool isQueue = tileElement->GetType() == TILE_ELEMENT_TYPE_PATH ? tileElement->AsPath()->IsQueue() : false;
            int32_t z0 = z1 - 2;
            footpath_remove_edges_towards(
                { footpathPos + CoordsDirectionDelta[direction], z0 * COORDS_Z_STEP, z1 * COORDS_Z_STEP }, direction, isQueue);
        }
        else
        {
            // A footpath may stay connected, but its edges must be fixed later on when another edge does get removed.
            fixCorners = true;
        }
    }

    // Only fix corners when needed, to avoid changing corners that have been set for its looks.
    if (fixCorners && tileElement->IsGhost())
    {
        auto tileFootpathPos = TileCoordsXY{ footpathPos };
        footpath_fix_corners_around(tileFootpathPos, tileElement);
    }

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        tileElement->AsPath()->SetEdgesAndCorners(0);
}

PathSurfaceEntry* get_path_surface_entry(PathSurfaceIndex entryIndex)
{
    PathSurfaceEntry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    // TODO: Change when moving to the new save format.
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_PATHS, entryIndex % MAX_PATH_OBJECTS);
    if (obj != nullptr)
    {
        if (entryIndex < MAX_PATH_OBJECTS)
            result = ((FootpathObject*)obj)->GetPathSurfaceEntry();
        else
            result = ((FootpathObject*)obj)->GetQueueEntry();
    }
    return result;
}

PathRailingsEntry* get_path_railings_entry(PathRailingsIndex entryIndex)
{
    PathRailingsEntry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_PATHS, entryIndex);
    if (obj != nullptr)
    {
        result = ((FootpathObject*)obj)->GetPathRailingsEntry();
    }
    return result;
}

ride_id_t PathElement::GetRideIndex() const
{
    return rideIndex;
}

void PathElement::SetRideIndex(ride_id_t newRideIndex)
{
    rideIndex = newRideIndex;
}

uint8_t PathElement::GetAdditionStatus() const
{
    return AdditionStatus;
}

void PathElement::SetAdditionStatus(uint8_t newStatus)
{
    AdditionStatus = newStatus;
}

uint8_t PathElement::GetEdges() const
{
    return Edges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
}

void PathElement::SetEdges(uint8_t newEdges)
{
    Edges &= ~FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
    Edges |= (newEdges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK);
}

uint8_t PathElement::GetCorners() const
{
    return Edges >> 4;
}

void PathElement::SetCorners(uint8_t newCorners)
{
    Edges &= ~FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK;
    Edges |= (newCorners << 4);
}

uint8_t PathElement::GetEdgesAndCorners() const
{
    return Edges;
}

void PathElement::SetEdgesAndCorners(uint8_t newEdgesAndCorners)
{
    Edges = newEdgesAndCorners;
}

bool PathElement::IsLevelCrossing(const CoordsXY& coords) const
{
    auto trackElement = map_get_track_element_at({ coords, GetBaseZ() });
    if (trackElement == nullptr)
    {
        return false;
    }

    if (trackElement->GetTrackType() != TRACK_ELEM_FLAT)
    {
        return false;
    }

    auto ride = get_ride(trackElement->GetRideIndex());
    if (ride == nullptr)
    {
        return false;
    }

    return (ride->type == RIDE_TYPE_MINIATURE_RAILWAY);
}

bool ExtendedPathData::IsWideForGroup(uint8_t wideGroup) const
{
    return IsWideForGroup(wideGroup, 0);
}

bool ExtendedPathData::IsWideForGroup(uint8_t wideGroup, uint8_t wideLevel) const
{
    if (WideFlags.size() > wideLevel)
        return (WideFlags[wideLevel] & (1 << wideGroup)) != 0;

    return false;
}

void ExtendedPathData::SetWideForGroup(uint8_t wideGroup, bool isWide)
{
    SetWideForGroup(wideGroup, 0, isWide);
}

void ExtendedPathData::SetWideForGroup(uint8_t wideGroup, uint8_t wideLevel, bool isWide)
{
    IncreasePathLevelTo(wideLevel + 1);
    WideFlags[wideLevel] &= ~(1 << wideGroup);
    if (isWide)
        WideFlags[wideLevel] |= (1 << wideGroup);
}

uint8_t ExtendedPathData::GetWideFlags() const
{
    return GetWideFlags(0);
}

uint8_t ExtendedPathData::GetWideFlags(uint8_t wideLevel) const
{
    if (WideFlags.size() > wideLevel)
        return WideFlags[wideLevel];

    return 0;
}

void ExtendedPathData::SetWideFlags(uint8_t flags)
{
    SetWideFlags(0, flags);
}

void ExtendedPathData::SetWideFlags(uint8_t wideLevel, uint8_t flags)
{
    IncreasePathLevelTo(wideLevel + 1);
    WideFlags[wideLevel] = flags;
}

int ExtendedPathData::GetPathLevel()
{
    return (int)WideFlags.size();
}

void ExtendedPathData::SetPathLevel(uint8_t pathLevel)
{
    IncreasePathLevelTo(pathLevel);
    DecreasePathLevelTo(pathLevel);
}

void ExtendedPathData::IncreasePathLevelTo(uint8_t pathLevel)
{
    while (GetPathLevel() < pathLevel)
    {
        WideFlags.push_back(0b00000000);
    }
}

void ExtendedPathData::DecreasePathLevelTo(uint8_t pathLevel)
{
    while (GetPathLevel() > pathLevel + 1)
    {
        WideFlags.pop_back();
    }
}

void ExtendedPathData::SetToZero()
{
    WideFlags.clear();
    WideFlags.push_back(0b00000000);
}
