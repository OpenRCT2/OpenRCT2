/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
#include "../actions/FootpathRemoveAction.hpp"
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

void footpath_update_queue_entrance_banner(int32_t x, int32_t y, TileElement* tileElement);

uint8_t gFootpathProvisionalFlags;
LocationXYZ16 gFootpathProvisionalPosition;
uint8_t gFootpathProvisionalType;
uint8_t gFootpathProvisionalSlope;
uint8_t gFootpathConstructionMode;
uint16_t gFootpathSelectedId;
uint8_t gFootpathSelectedType;
LocationXYZ16 gFootpathConstructFromPosition;
uint8_t gFootpathConstructDirection;
uint8_t gFootpathConstructSlope;
uint8_t gFootpathConstructValidDirections;
money32 gFootpathPrice;
uint8_t gFootpathGroundFlags;

static uint8_t* _footpathQueueChainNext;
static uint8_t _footpathQueueChain[64];

// This is the coordinates that a user of the bin should move to
// rct2: 0x00992A4C
const LocationXY16 BinUseOffsets[4] = {
    { 11, 16 },
    { 16, 21 },
    { 21, 16 },
    { 16, 11 },
};

// These are the offsets for bench positions on footpaths, 2 for each edge
// rct2: 0x00981F2C, 0x00981F2E
const LocationXY16 BenchUseOffsets[8] = {
    { 7, 12 }, { 12, 25 }, { 25, 20 }, { 20, 7 }, { 7, 20 }, { 20, 25 }, { 25, 12 }, { 12, 7 },
};

/** rct2: 0x00981D6C, 0x00981D6E */
const LocationXY16 word_981D6C[4] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };

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

/**
 *
 *  rct2: 0x006A65AD
 */
static void automatically_set_peep_spawn(CoordsXYZ location)
{
    uint8_t direction = 0;
    if (location.x != 32)
    {
        direction++;
        if (location.y != gMapSizeUnits - 32)
        {
            direction++;
            if (location.x != gMapSizeUnits - 32)
            {
                direction++;
                if (location.y != 32)
                    return;
            }
        }
    }

    if (gPeepSpawns.size() == 0)
    {
        gPeepSpawns.emplace_back();
    }
    PeepSpawn* peepSpawn = &gPeepSpawns[0];
    peepSpawn->x = location.x + (word_981D6C[direction].x * 15) + 16;
    peepSpawn->y = location.y + (word_981D6C[direction].y * 15) + 16;
    peepSpawn->direction = direction;
    peepSpawn->z = location.z;
}

TileElement* map_get_footpath_element(int32_t x, int32_t y, int32_t z)
{
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x, y);
    do
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH && tileElement->base_height == z)
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

static TileElement* map_get_footpath_element_slope(int32_t x, int32_t y, int32_t z, int32_t slope)
{
    TileElement* tileElement;
    bool isSloped = slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;

    tileElement = map_get_first_element_at(x, y);
    do
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH && tileElement->base_height == z
            && (tileElement->AsPath()->IsSloped() == isSloped)
            && (tileElement->AsPath()->GetSlopeDirection() == (slope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)))
        {
            return tileElement;
        }
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

static void loc_6A6620(int32_t flags, int32_t x, int32_t y, TileElement* tileElement)
{
    if (tileElement->AsPath()->IsSloped() && !(flags & GAME_COMMAND_FLAG_GHOST))
    {
        int32_t direction = tileElement->AsPath()->GetSlopeDirection();
        int32_t z = tileElement->base_height;
        wall_remove_intersecting_walls(x, y, z, z + 6, direction_reverse(direction));
        wall_remove_intersecting_walls(x, y, z, z + 6, direction);
        // Removing walls may have made the pointer invalid, so find it again
        tileElement = map_get_footpath_element(x / 32, y / 32, z);
    }

    if (!(flags & GAME_COMMAND_FLAG_PATH_SCENERY))
        footpath_connect_edges(x, y, tileElement, flags);

    footpath_update_queue_chains();
    map_invalidate_tile_full(x, y);
}

/** rct2: 0x0098D7EC */
static constexpr const QuarterTile SlopedFootpathQuarterTiles[] = {
    { 0b1111, 0b1100 }, { 0b1111, 0b1001 }, { 0b1111, 0b0011 }, { 0b1111, 0b0110 }
};

static money32 footpath_element_insert(
    int32_t type, int32_t x, int32_t y, int32_t z, int32_t slope, int32_t flags, uint8_t pathItemType)
{
    TileElement* tileElement;
    EntranceElement* entranceElement;
    int32_t zHigh;
    bool entrancePath = false, entranceIsSamePath = false;

    if (!map_check_free_elements_and_reorganise(1))
        return MONEY32_UNDEFINED;

    if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST)))
        footpath_remove_litter(x, y, gCommandPosition.z);

    // loc_6A649D:
    gFootpathPrice += MONEY(12, 00);

    QuarterTile quarterTile{ 0b1111, 0 };
    zHigh = z + 4;
    if (slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
    {
        quarterTile = SlopedFootpathQuarterTiles[slope & TILE_ELEMENT_DIRECTION_MASK];
        zHigh += 2;
    }

    entranceElement = map_get_park_entrance_element_at(x, y, z, false);
    // Make sure the entrance part is the middle
    if (entranceElement != nullptr && (entranceElement->GetSequenceIndex()) == 0)
    {
        entrancePath = true;
        // Make the price the same as replacing a path
        if (entranceElement->GetPathType() == (type & 0xF))
            entranceIsSamePath = true;
        else
            gFootpathPrice -= MONEY(6, 00);
    }

    // Do not attempt to build a crossing with a queue or a sloped.
    uint8_t crossingMode = (type & FOOTPATH_ELEMENT_INSERT_QUEUE) || (slope != TILE_ELEMENT_SLOPE_FLAT)
        ? CREATE_CROSSING_MODE_NONE
        : CREATE_CROSSING_MODE_PATH_OVER_TRACK;
    if (!entrancePath
        && !map_can_construct_with_clear_at(
               x, y, z, zHigh, &map_place_non_scenery_clear_func, quarterTile, flags, &gFootpathPrice, crossingMode))
        return MONEY32_UNDEFINED;

    gFootpathGroundFlags = gMapGroundFlags;
    if (!gCheatsDisableClearanceChecks && (gMapGroundFlags & ELEMENT_IS_UNDERWATER))
    {
        gGameCommandErrorText = STR_CANT_BUILD_THIS_UNDERWATER;
        return MONEY32_UNDEFINED;
    }

    tileElement = map_get_surface_element_at({ x, y });

    if (tileElement == nullptr)
    {
        return MONEY32_UNDEFINED;
    }

    int32_t supportHeight = z - tileElement->base_height;
    gFootpathPrice += supportHeight < 0 ? MONEY(20, 00) : (supportHeight / 2) * MONEY(5, 00);

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (entrancePath)
        {
            if (!(flags & GAME_COMMAND_FLAG_GHOST) && !entranceIsSamePath)
            {
                // Set the path type but make sure it's not a queue as that will not show up
                entranceElement->SetPathType(type & 0x7F);
                map_invalidate_tile_full(x, y);
            }
        }
        else
        {
            tileElement = tile_element_insert(x / 32, y / 32, z, 0x0F);
            assert(tileElement != nullptr);
            tileElement->SetType(TILE_ELEMENT_TYPE_PATH);
            PathElement* pathElement = tileElement->AsPath();
            pathElement->clearance_height = z + 4 + ((slope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK) ? 2 : 0);
            pathElement->SetPathEntryIndex(type);
            pathElement->SetSlopeDirection(slope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK);
            if (slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
                pathElement->SetSloped(true);
            if (type & FOOTPATH_ELEMENT_INSERT_QUEUE)
                pathElement->SetIsQueue(true);
            pathElement->SetAddition(pathItemType);
            pathElement->SetRideIndex(RIDE_ID_NULL);
            pathElement->SetAdditionStatus(255);
            pathElement->SetIsBroken(false);
            if (flags & GAME_COMMAND_FLAG_GHOST)
                pathElement->SetGhost(true);

            footpath_queue_chain_reset();

            if (!(flags & GAME_COMMAND_FLAG_PATH_SCENERY))
                footpath_remove_edges_at(x, y, tileElement);

            if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(flags & GAME_COMMAND_FLAG_GHOST))
                automatically_set_peep_spawn({ x, y, tileElement->base_height * 8 });

            loc_6A6620(flags, x, y, tileElement);
        }
    }

    // Prevent the place sound from being spammed
    if (entranceIsSamePath)
        gFootpathPrice = 0;

    return gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : gFootpathPrice;
}

static money32 footpath_element_update(
    int32_t x, int32_t y, TileElement* tileElement, int32_t type, int32_t flags, uint8_t pathItemType)
{
    const int32_t newFootpathType = (type & (FOOTPATH_PROPERTIES_TYPE_MASK >> 4));
    const bool newPathIsQueue = ((type >> 7) == 1);

    if (tileElement->AsPath()->GetPathEntryIndex() != newFootpathType || tileElement->AsPath()->IsQueue() != newPathIsQueue)
    {
        gFootpathPrice += MONEY(6, 00);
    }
    else if (pathItemType != 0)
    {
        Guard::Assert(false, "No longer in use. Use FootpathScenery{Place/Remove}Action.");
    }

    if (flags & GAME_COMMAND_FLAG_4)
        return MONEY32_UNDEFINED;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        footpath_queue_chain_reset();

        if (!(flags & GAME_COMMAND_FLAG_PATH_SCENERY))
            footpath_remove_edges_at(x, y, tileElement);

        tileElement->AsPath()->SetPathEntryIndex(type);
        if (type & (1 << 7))
            tileElement->AsPath()->SetIsQueue(true);
        else
            tileElement->AsPath()->SetIsQueue(false);
        tileElement->AsPath()->SetAddition(pathItemType);
        tileElement->AsPath()->SetIsBroken(false);

        loc_6A6620(flags, x, y, tileElement);
    }

    return gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : gFootpathPrice;
}

static money32 footpath_place_real(
    int32_t type, int32_t x, int32_t y, int32_t z, int32_t slope, int32_t flags, uint8_t pathItemType, bool clearDirection,
    int32_t direction)
{
    TileElement* tileElement;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z * 8;

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST))
        footpath_interrupt_peeps(x, y, z * 8);

    gFootpathPrice = 0;
    gFootpathGroundFlags = 0;

    if (map_is_edge({ x, y }))
    {
        gGameCommandErrorText = STR_OFF_EDGE_OF_MAP;
        return MONEY32_UNDEFINED;
    }

    if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(x, y, z * 8))
        return MONEY32_UNDEFINED;

    if (slope & SLOPE_IS_IRREGULAR_FLAG)
    {
        gGameCommandErrorText = STR_LAND_SLOPE_UNSUITABLE;
        return MONEY32_UNDEFINED;
    }

    if (z < 2)
    {
        gGameCommandErrorText = STR_TOO_LOW;
        return MONEY32_UNDEFINED;
    }

    if (z > 248)
    {
        gGameCommandErrorText = STR_TOO_HIGH;
        return MONEY32_UNDEFINED;
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST))
    {
        LocationXYZ16 coord;
        coord.x = x + 16;
        coord.y = y + 16;
        coord.z = tile_element_height(coord.x, coord.y);
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);

        if (clearDirection && !gCheatsDisableClearanceChecks)
        {
            direction = direction & 0xF;
            // It is possible, let's remove walls between the old and new piece of path
            wall_remove_intersecting_walls(
                x, y, z, z + 4 + ((slope & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK) ? 2 : 0), direction_reverse(direction));
            wall_remove_intersecting_walls(
                x - CoordsDirectionDelta[direction].x, y - CoordsDirectionDelta[direction].y, z, z + 4, direction);
        }
    }

    footpath_provisional_remove();
    tileElement = map_get_footpath_element_slope((x / 32), (y / 32), z, slope);
    if (tileElement == nullptr)
    {
        return footpath_element_insert(type, x, y, z, slope, flags, pathItemType);
    }
    else
    {
        return footpath_element_update(x, y, tileElement, type, flags, pathItemType);
    }
}

/**
 *
 *  rct2: 0x006BA23E
 */
void remove_banners_at_element(int32_t x, int32_t y, TileElement* tileElement)
{
    while (!(tileElement++)->IsLastForTile())
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
            return;
        else if (tileElement->GetType() != TILE_ELEMENT_TYPE_BANNER)
            continue;

        game_do_command(
            x, 1, y, tileElement->base_height | tileElement->AsBanner()->GetPosition() << 8, GAME_COMMAND_REMOVE_BANNER, 0, 0);
        tileElement--;
    }
}

/**
 *
 *  rct2: 0x006A61DE
 */
void game_command_place_footpath(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    *ebx = footpath_place_real(
        (*edx >> 8) & 0xFF, *eax & 0xFFFF, *ecx & 0xFFFF, *edx & 0xFF, (*ebx >> 8) & 0xFF, *ebx & 0xFF, *edi & 0xFF,
        (*ebp & FOOTPATH_CLEAR_DIRECTIONAL) >> 8, *ebp & 0xFF);
}

static money32 footpath_place_from_track(
    int32_t type, int32_t x, int32_t y, int32_t z, int32_t slope, int32_t edges, int32_t flags)
{
    TileElement* tileElement;
    EntranceElement* entranceElement;
    bool entrancePath = false, entranceIsSamePath = false;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z * 8;

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST))
        footpath_interrupt_peeps(x, y, z * 8);

    gFootpathPrice = 0;
    gFootpathGroundFlags = 0;

    if (!map_is_location_owned(x, y, z * 8) && !gCheatsSandboxMode)
    {
        return MONEY32_UNDEFINED;
    }

    if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(x, y, z * 8))
        return MONEY32_UNDEFINED;

    if (z < 2)
    {
        gGameCommandErrorText = STR_TOO_LOW;
        return MONEY32_UNDEFINED;
    }

    if (z > 248)
    {
        gGameCommandErrorText = STR_TOO_HIGH;
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (!(flags & (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST)))
        {
            footpath_remove_litter(x, y, z * 8);
        }
    }

    gFootpathPrice += 120;
    QuarterTile quarterTile = { 0b1111, 0 };
    int32_t zHigh = z + 4;
    if (slope & TILE_ELEMENT_SLOPE_S_CORNER_UP)
    {
        quarterTile = SlopedFootpathQuarterTiles[slope & TILE_ELEMENT_SLOPE_NE_SIDE_UP];
        zHigh += 2;
    }

    entranceElement = map_get_park_entrance_element_at(x, y, z, false);
    // Make sure the entrance part is the middle
    if (entranceElement != nullptr && (entranceElement->GetSequenceIndex()) == 0)
    {
        entrancePath = true;
        // Make the price the same as replacing a path
        if (entranceElement->GetPathType() == (type & 0xF))
            entranceIsSamePath = true;
        else
            gFootpathPrice -= MONEY(6, 00);
    }

    // Do not attempt to build a crossing with a queue or a sloped.
    uint8_t crossingMode = (type & FOOTPATH_ELEMENT_INSERT_QUEUE) || (slope != TILE_ELEMENT_SLOPE_FLAT)
        ? CREATE_CROSSING_MODE_NONE
        : CREATE_CROSSING_MODE_PATH_OVER_TRACK;
    if (!entrancePath
        && !map_can_construct_with_clear_at(
               x, y, z, zHigh, &map_place_non_scenery_clear_func, quarterTile, flags, &gFootpathPrice, crossingMode))
        return MONEY32_UNDEFINED;

    gFootpathGroundFlags = gMapGroundFlags;
    if (!gCheatsDisableClearanceChecks && (gMapGroundFlags & ELEMENT_IS_UNDERWATER))
    {
        gGameCommandErrorText = STR_CANT_BUILD_THIS_UNDERWATER;
        return MONEY32_UNDEFINED;
    }

    tileElement = map_get_surface_element_at({ x, y });

    int32_t supportHeight = z - tileElement->base_height;
    gFootpathPrice += supportHeight < 0 ? MONEY(20, 00) : (supportHeight / 2) * MONEY(5, 00);

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            LocationXYZ16 coord;
            coord.x = x + 16;
            coord.y = y + 16;
            coord.z = tile_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        if (entrancePath)
        {
            if (!(flags & GAME_COMMAND_FLAG_GHOST) && !entranceIsSamePath)
            {
                // Set the path type but make sure it's not a queue as that will not show up
                entranceElement->SetPathType(type & 0x7F);
                map_invalidate_tile_full(x, y);
            }
        }
        else
        {
            tileElement = tile_element_insert(x / 32, y / 32, z, 0x0F);
            assert(tileElement != nullptr);
            tileElement->SetType(TILE_ELEMENT_TYPE_PATH);
            PathElement* pathElement = tileElement->AsPath();
            // This can NEVER happen, but GCC does not want to believe that...
            if (pathElement == nullptr)
            {
                assert(false);
                return MONEY32_UNDEFINED;
            }
            pathElement->clearance_height = z + 4 + ((slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED) ? 2 : 0);
            pathElement->SetPathEntryIndex(type & 0xF);
            pathElement->SetSlopeDirection(slope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK);
            if (slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
                pathElement->SetSloped(true);
            if (type & (1 << 7))
                pathElement->SetIsQueue(true);
            pathElement->SetAddition(0);
            pathElement->SetRideIndex(RIDE_ID_NULL);
            pathElement->SetAdditionStatus(255);
            pathElement->SetEdges(edges);
            pathElement->SetCorners(0);
            pathElement->SetIsBroken(false);
            if (flags & (1 << 6))
                pathElement->SetGhost(true);

            map_invalidate_tile_full(x, y);
        }
    }

    if (entranceIsSamePath)
        gFootpathPrice = 0;

    return gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : gFootpathPrice;
}

/**
 *
 *  rct2: 0x006A68AE
 */
void game_command_place_footpath_from_track(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, [[maybe_unused]] int32_t* edi,
    [[maybe_unused]] int32_t* ebp)
{
    *ebx = footpath_place_from_track(
        (*edx >> 8) & 0xFF, *eax & 0xFFFF, *ecx & 0xFFFF, *edx & 0xFF, ((*ebx >> 13) & 0x3) | ((*ebx >> 10) & 0x4),
        (*ebx >> 8) & 0xF, *ebx & 0xFF);
}

money32 footpath_place(int32_t type, int32_t x, int32_t y, int32_t z, int32_t slope, int32_t flags)
{
    return game_do_command(x, (slope << 8) | flags, y, (type << 8) | z, GAME_COMMAND_PLACE_PATH, 0, 0);
}

money32 footpath_place_remove_intersecting(
    int32_t type, int32_t x, int32_t y, int32_t z, int32_t slope, int32_t flags, int32_t direction)
{
    return game_do_command(
        x, (slope << 8) | flags, y, (type << 8) | z, GAME_COMMAND_PLACE_PATH, 0, FOOTPATH_CLEAR_DIRECTIONAL | direction);
}

money32 footpath_remove(int32_t x, int32_t y, int32_t z, int32_t flags)
{
    auto action = FootpathRemoveAction(x, y, z);
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
money32 footpath_provisional_set(int32_t type, int32_t x, int32_t y, int32_t z, int32_t slope)
{
    money32 cost;

    footpath_provisional_remove();

    cost = footpath_place(
        type, x, y, z, slope,
        GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_4 | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
            | GAME_COMMAND_FLAG_APPLY);
    if (cost != MONEY32_UNDEFINED)
    {
        gFootpathProvisionalType = type;
        gFootpathProvisionalPosition.x = x;
        gFootpathProvisionalPosition.y = y;
        gFootpathProvisionalPosition.z = z & 0xFF;
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
        if (cost == MONEY32_UNDEFINED)
        {
            // If we can't build this, don't show a virtual floor.
            virtual_floor_set_height(0);
        }
        else if (
            gFootpathConstructSlope == TILE_ELEMENT_SLOPE_FLAT
            || gFootpathProvisionalPosition.z * 8 < gFootpathConstructFromPosition.z)
        {
            // Going either straight on, or down.
            virtual_floor_set_height(gFootpathProvisionalPosition.z * 8);
        }
        else
        {
            // Going up in the world!
            virtual_floor_set_height((gFootpathProvisionalPosition.z + 2) * 8);
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
            gFootpathProvisionalPosition.x, gFootpathProvisionalPosition.y, gFootpathProvisionalPosition.z,
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_GHOST);
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
        map_invalidate_tile_full(gFootpathConstructFromPosition.x, gFootpathConstructFromPosition.y);
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
void footpath_get_coordinates_from_pos(
    int32_t screenX, int32_t screenY, int32_t* x, int32_t* y, int32_t* direction, TileElement** tileElement)
{
    int32_t z = 0, interactionType;
    TileElement* myTileElement;
    rct_viewport* viewport;
    LocationXY16 position = {};

    get_map_coordinates_from_pos(
        screenX, screenY, VIEWPORT_INTERACTION_MASK_FOOTPATH, &position.x, &position.y, &interactionType, &myTileElement,
        &viewport);
    if (interactionType != VIEWPORT_INTERACTION_ITEM_FOOTPATH
        || !(viewport->flags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL)))
    {
        get_map_coordinates_from_pos(
            screenX, screenY, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &position.x, &position.y,
            &interactionType, &myTileElement, &viewport);
        if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE)
        {
            if (x != nullptr)
                *x = LOCATION_NULL;
            return;
        }
    }

    LocationXY16 minPosition = position;
    LocationXY16 maxPosition = { int16_t(position.x + 31), int16_t(position.y + 31) };

    position.x += 16;
    position.y += 16;

    if (interactionType == VIEWPORT_INTERACTION_ITEM_FOOTPATH)
    {
        z = myTileElement->base_height * 8;
        if (myTileElement->AsPath()->IsSloped())
        {
            z += 8;
        }
    }

    LocationXY16 start_vp_pos = screen_coord_to_viewport_coord(viewport, screenX, screenY);

    for (int32_t i = 0; i < 5; i++)
    {
        if (interactionType != VIEWPORT_INTERACTION_ITEM_FOOTPATH)
        {
            z = tile_element_height(position.x, position.y);
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

    if (x != nullptr)
        *x = position.x & ~0x1F;
    if (y != nullptr)
        *y = position.y & ~0x1F;
    if (direction != nullptr)
        *direction = myDirection;
    if (tileElement != nullptr)
        *tileElement = myTileElement;
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
void footpath_bridge_get_info_from_pos(
    int32_t screenX, int32_t screenY, int32_t* x, int32_t* y, int32_t* direction, TileElement** tileElement)
{
    // First check if we point at an entrance or exit. In that case, we would want the path coming from the entrance/exit.
    int32_t interactionType;
    rct_viewport* viewport;

    LocationXY16 map_pos = {};
    get_map_coordinates_from_pos(
        screenX, screenY, VIEWPORT_INTERACTION_MASK_RIDE, &map_pos.x, &map_pos.y, &interactionType, tileElement, &viewport);
    *x = map_pos.x;
    *y = map_pos.y;

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
            return;
        }
    }

    get_map_coordinates_from_pos(
        screenX, screenY,
        VIEWPORT_INTERACTION_MASK_RIDE & VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &map_pos.x,
        &map_pos.y, &interactionType, tileElement, &viewport);
    *x = map_pos.x;
    *y = map_pos.y;
    if (interactionType == VIEWPORT_INTERACTION_ITEM_RIDE && (*tileElement)->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
    {
        int32_t directions = entrance_get_directions(*tileElement);
        if (directions & 0x0F)
        {
            int32_t bx = (*tileElement)->GetDirectionWithOffset(bitscanforward(directions));
            if (direction != nullptr)
                *direction = bx;
            return;
        }
    }

    // We point at something else
    footpath_get_coordinates_from_pos(screenX, screenY, x, y, direction, tileElement);
}

/**
 *
 *  rct2: 0x00673883
 */
void footpath_remove_litter(int32_t x, int32_t y, int32_t z)
{
    uint16_t spriteIndex = sprite_get_first_in_quadrant(x, y);
    while (spriteIndex != SPRITE_INDEX_NULL)
    {
        rct_litter* sprite = &get_sprite(spriteIndex)->litter;
        uint16_t nextSpriteIndex = sprite->next_in_quadrant;
        if (sprite->linked_list_type_offset == SPRITE_LIST_LITTER * 2)
        {
            int32_t distanceZ = abs(sprite->z - z);
            if (distanceZ <= 32)
            {
                invalidate_sprite_0((rct_sprite*)sprite);
                sprite_remove((rct_sprite*)sprite);
            }
        }
        spriteIndex = nextSpriteIndex;
    }
}

/**
 *
 *  rct2: 0x0069A48B
 */
void footpath_interrupt_peeps(int32_t x, int32_t y, int32_t z)
{
    uint16_t spriteIndex = sprite_get_first_in_quadrant(x, y);
    while (spriteIndex != SPRITE_INDEX_NULL)
    {
        Peep* peep = &get_sprite(spriteIndex)->peep;
        uint16_t nextSpriteIndex = peep->next_in_quadrant;
        if (peep->linked_list_type_offset == SPRITE_LIST_PEEP * 2)
        {
            if (peep->state == PEEP_STATE_SITTING || peep->state == PEEP_STATE_WATCHING)
            {
                if (peep->z == z)
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
 *
 *  rct2: 0x006E59DC
 */
bool fence_in_the_way(int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t direction)
{
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
        return false;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (tileElement->IsGhost())
            continue;
        if (z0 >= tileElement->clearance_height)
            continue;
        if (z1 <= tileElement->base_height)
            continue;
        if ((tileElement->GetDirection()) != direction)
            continue;

        return true;
    } while (!(tileElement++)->IsLastForTile());
    return false;
}

static TileElement* footpath_connect_corners_get_neighbour(int32_t x, int32_t y, int32_t z, int32_t requireEdges)
{
    if (!map_is_location_valid({ x, y }))
    {
        return nullptr;
    }

    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (tileElement->AsPath()->IsQueue())
            continue;
        if (tileElement->base_height != z)
            continue;
        if (!(tileElement->AsPath()->GetEdgesAndCorners() & requireEdges))
            continue;

        return tileElement;
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
static void footpath_connect_corners(int32_t initialX, int32_t initialY, TileElement* initialTileElement)
{
    TileElement* tileElement[4];

    if (initialTileElement->AsPath()->IsQueue())
        return;
    if (initialTileElement->AsPath()->IsSloped())
        return;

    tileElement[0] = initialTileElement;
    int32_t z = initialTileElement->base_height;
    for (int32_t initialDirection = 0; initialDirection < 4; initialDirection++)
    {
        int32_t x = initialX;
        int32_t y = initialY;
        int32_t direction = initialDirection;

        x += CoordsDirectionDelta[direction].x;
        y += CoordsDirectionDelta[direction].y;
        tileElement[1] = footpath_connect_corners_get_neighbour(x, y, z, (1 << direction_reverse(direction)));
        if (tileElement[1] == nullptr)
            continue;

        direction = (direction + 1) & 3;
        x += CoordsDirectionDelta[direction].x;
        y += CoordsDirectionDelta[direction].y;
        tileElement[2] = footpath_connect_corners_get_neighbour(x, y, z, (1 << direction_reverse(direction)));
        if (tileElement[2] == nullptr)
            continue;

        direction = (direction + 1) & 3;
        x += CoordsDirectionDelta[direction].x;
        y += CoordsDirectionDelta[direction].y;
        // First check link to previous tile
        tileElement[3] = footpath_connect_corners_get_neighbour(x, y, z, (1 << direction_reverse(direction)));
        if (tileElement[3] == nullptr)
            continue;
        // Second check link to initial tile
        tileElement[3] = footpath_connect_corners_get_neighbour(x, y, z, (1 << ((direction + 1) & 3)));
        if (tileElement[3] == nullptr)
            continue;

        direction = (direction + 1) & 3;
        tileElement[3]->AsPath()->SetCorners(tileElement[3]->AsPath()->GetCorners() | (1 << (direction)));
        map_invalidate_element(x, y, tileElement[3]);

        direction = (direction - 1) & 3;
        tileElement[2]->AsPath()->SetCorners(tileElement[2]->AsPath()->GetCorners() | (1 << (direction)));

        map_invalidate_element(x, y, tileElement[2]);

        direction = (direction - 1) & 3;
        tileElement[1]->AsPath()->SetCorners(tileElement[1]->AsPath()->GetCorners() | (1 << (direction)));

        map_invalidate_element(x, y, tileElement[1]);

        direction = initialDirection;
        tileElement[0]->AsPath()->SetCorners(tileElement[0]->AsPath()->GetCorners() | (1 << (direction)));
        map_invalidate_element(x, y, tileElement[0]);
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

static TileElement* footpath_get_element(int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t direction)
{
    TileElement* tileElement;
    int32_t slope;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (z1 == tileElement->base_height)
        {
            if (tileElement->AsPath()->IsSloped())
            {
                slope = tileElement->AsPath()->GetSlopeDirection();
                if (slope != direction)
                    break;
            }
            return tileElement;
        }
        if (z0 == tileElement->base_height)
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

static bool sub_footpath_disconnect_queue_from_path(
    int32_t x, int32_t y, TileElement* tileElement, int32_t action, int32_t direction)
{
    if (((tileElement->AsPath()->GetEdges() & (1 << direction)) == 0) ^ (action < 0))
        return false;
    if ((action < 0) && fence_in_the_way(x, y, tileElement->base_height, tileElement->clearance_height, direction))
        return false;

    int32_t x1 = x + CoordsDirectionDelta[direction].x;
    int32_t y1 = y + CoordsDirectionDelta[direction].y;
    int32_t z = tileElement->base_height;
    TileElement* otherTileElement = footpath_get_element(x1, y1, z - 2, z, direction);
    if (otherTileElement != nullptr && !otherTileElement->AsPath()->IsQueue())
    {
        tileElement->AsPath()->SetSlopeDirection(0);
        if (action > 0)
        {
            tileElement->AsPath()->SetEdges(tileElement->AsPath()->GetEdges() & ~(1 << direction));
            otherTileElement->AsPath()->SetEdges(otherTileElement->AsPath()->GetEdges() & ~(1 << ((direction + 2) & 3)));
            if (action >= 2)
                tileElement->AsPath()->SetSlopeDirection(direction);
        }
        else if (action < 0)
        {
            tileElement->AsPath()->SetEdges(tileElement->AsPath()->GetEdges() | (1 << direction));
            otherTileElement->AsPath()->SetEdges(otherTileElement->AsPath()->GetEdges() | (1 << ((direction + 2) & 3)));
        }
        if (action != 0)
            map_invalidate_tile_full(x1, y1);
        return true;
    }
    return false;
}

static bool footpath_disconnect_queue_from_path(int32_t x, int32_t y, TileElement* tileElement, int32_t action)
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
        if (sub_footpath_disconnect_queue_from_path(x, y, tileElement, action, direction))
            return true;
    }

    for (int32_t direction = 0; direction < 4; direction++)
    {
        if ((action < 0) && (direction == tileElement->AsPath()->GetSlopeDirection()))
            continue;
        if (sub_footpath_disconnect_queue_from_path(x, y, tileElement, action, direction))
            return true;
    }

    return false;
}

/**
 *
 *  rct2: 0x006A6D7E
 */
static void loc_6A6D7E(
    int32_t initialX, int32_t initialY, int32_t z, int32_t direction, TileElement* initialTileElement, int32_t flags,
    bool query, rct_neighbour_list* neighbourList)
{
    int32_t x = initialX + CoordsDirectionDelta[direction].x;
    int32_t y = initialY + CoordsDirectionDelta[direction].y;
    if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && map_is_edge({ x, y }))
    {
        if (query)
        {
            neighbour_list_push(neighbourList, 7, direction, 255, 255);
        }
    }
    else
    {
        TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
        do
        {
            switch (tileElement->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                    if (z == tileElement->base_height)
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
                    if (z - 2 == tileElement->base_height)
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
                    if (z == tileElement->base_height)
                    {
                        Ride* ride = get_ride(tileElement->AsTrack()->GetRideIndex());
                        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                        {
                            continue;
                        }

                        const uint8_t trackType = tileElement->AsTrack()->GetTrackType();
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
                    if (z == tileElement->base_height)
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
            if (fence_in_the_way(x, y, tileElement->base_height, tileElement->clearance_height, direction_reverse(direction)))
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
                        if (footpath_disconnect_queue_from_path(x, y, tileElement, 0))
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
            footpath_disconnect_queue_from_path(x, y, tileElement, 1 + ((flags >> 6) & 1));
            tileElement->AsPath()->SetEdges(tileElement->AsPath()->GetEdges() | (1 << direction_reverse(direction)));
            if (tileElement->AsPath()->IsQueue())
            {
                footpath_queue_chain_push(tileElement->AsPath()->GetRideIndex());
            }
        }
        if (!(flags & (GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)))
        {
            footpath_interrupt_peeps(x, y, tileElement->base_height * 8);
        }
        map_invalidate_element(x, y, tileElement);
    }

loc_6A6FD2:
    if ((initialTileElement)->GetType() == TILE_ELEMENT_TYPE_PATH)
    {
        if (!query)
        {
            initialTileElement->AsPath()->SetEdges(initialTileElement->AsPath()->GetEdges() | (1 << direction));
            map_invalidate_element(initialX, initialY, initialTileElement);
        }
    }
}

static void loc_6A6C85(
    int32_t x, int32_t y, int32_t direction, TileElement* tileElement, int32_t flags, bool query,
    rct_neighbour_list* neighbourList)
{
    if (query && fence_in_the_way(x, y, tileElement->base_height, tileElement->clearance_height, direction))
        return;

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
    {
        if (!entrance_has_direction(tileElement, direction - tileElement->GetDirection()))
        {
            return;
        }
    }

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
    {
        Ride* ride = get_ride(tileElement->AsTrack()->GetRideIndex());
        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            return;
        }
        const uint8_t trackType = tileElement->AsTrack()->GetTrackType();
        const uint8_t trackSequence = tileElement->AsTrack()->GetSequenceIndex();
        if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH))
        {
            return;
        }
        uint16_t dx = (direction - tileElement->GetDirection()) & TILE_ELEMENT_DIRECTION_MASK;
        if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & (1 << dx)))
        {
            return;
        }
    }

    int32_t z = tileElement->base_height;
    if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
    {
        if (tileElement->AsPath()->IsSloped())
        {
            if ((tileElement->AsPath()->GetSlopeDirection() - direction) & 1)
            {
                return;
            }
            if (tileElement->AsPath()->GetSlopeDirection() == direction)
            {
                z += 2;
            }
        }
    }

    loc_6A6D7E(x, y, z, direction, tileElement, flags, query, neighbourList);
}

/**
 *
 *  rct2: 0x006A6C66
 */
void footpath_connect_edges(int32_t x, int32_t y, TileElement* tileElement, int32_t flags)
{
    rct_neighbour_list neighbourList;
    rct_neighbour neighbour;

    footpath_update_queue_chains();

    neighbour_list_init(&neighbourList);

    footpath_update_queue_entrance_banner(x, y, tileElement);
    for (int32_t direction = 0; direction < 4; direction++)
    {
        loc_6A6C85(x, y, direction, tileElement, flags, true, &neighbourList);
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
        loc_6A6C85(x, y, neighbour.direction, tileElement, flags, false, nullptr);
    }

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
    {
        footpath_connect_corners(x, y, tileElement);
    }
}

/**
 *
 *  rct2: 0x006A742F
 */
void footpath_chain_ride_queue(
    ride_id_t rideIndex, int32_t entranceIndex, int32_t x, int32_t y, TileElement* tileElement, int32_t direction)
{
    TileElement *lastPathElement, *lastQueuePathElement;
    int32_t lastPathX = x, lastPathY = y, lastPathDirection = direction;

    lastPathElement = nullptr;
    lastQueuePathElement = nullptr;
    int32_t z = tileElement->base_height;
    for (;;)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            lastPathElement = tileElement;
            lastPathX = x;
            lastPathY = y;
            lastPathDirection = direction;
            if (tileElement->AsPath()->IsSloped())
            {
                if (tileElement->AsPath()->GetSlopeDirection() == direction)
                {
                    z += 2;
                }
            }
        }

        x += CoordsDirectionDelta[direction].x;
        y += CoordsDirectionDelta[direction].y;
        tileElement = map_get_first_element_at(x >> 5, y >> 5);
        do
        {
            if (lastQueuePathElement == tileElement)
                continue;
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
                continue;
            if (tileElement->base_height == z)
            {
                if (tileElement->AsPath()->IsSloped())
                {
                    if (tileElement->AsPath()->GetSlopeDirection() != direction)
                        break;
                }
                goto foundNextPath;
            }
            if (tileElement->base_height == z - 2)
            {
                if (!tileElement->AsPath()->IsSloped())
                    break;

                if (direction_reverse(tileElement->AsPath()->GetSlopeDirection()) != direction)
                    break;

                z -= 2;
                goto foundNextPath;
            }
        } while (!(tileElement++)->IsLastForTile());
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

            map_invalidate_element(x, y, tileElement);

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

            map_animation_create(MAP_ANIMATION_TYPE_QUEUE_BANNER, lastPathX, lastPathY, lastPathElement->base_height);
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
        Ride* ride = get_ride(rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
            continue;

        for (int32_t i = 0; i < MAX_STATIONS; i++)
        {
            TileCoordsXYZD location = ride_get_entrance_location(ride, i);
            if (location.isNull())
                continue;

            TileElement* tileElement = map_get_first_element_at(location.x, location.y);
            do
            {
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                    continue;
                if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
                    continue;
                if (tileElement->AsEntrance()->GetRideIndex() != rideIndex)
                    continue;

                uint8_t direction = tileElement->GetDirectionWithOffset(2);
                footpath_chain_ride_queue(rideIndex, i, location.x << 5, location.y << 5, tileElement, direction);
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}

/**
 *
 *  rct2: 0x0069ADBD
 */
static void footpath_fix_ownership(int32_t x, int32_t y)
{
    const TileElement* surfaceElement = map_get_surface_element_at({ x, y });
    uint16_t ownership;

    // Unlikely to be NULL unless deliberate.
    if (surfaceElement != nullptr)
    {
        // If the tile is not safe to own construction rights of, erase them.
        if (check_max_allowable_land_rights_for_tile(x >> 5, y >> 5, surfaceElement->base_height) == OWNERSHIP_UNOWNED)
        {
            ownership = OWNERSHIP_UNOWNED;
        }
        // If the tile is safe to own construction rights of, do not erase contruction rights.
        else
        {
            ownership = surfaceElement->AsSurface()->GetOwnership();
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

    map_buy_land_rights(x, y, x, y, BUY_LAND_RIGHTS_FLAG_SET_OWNERSHIP_WITH_CHECKS, (ownership << 4) | GAME_COMMAND_FLAG_APPLY);
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
    int32_t x, int32_t y, int32_t z, int32_t direction, int32_t flags, int32_t level, int32_t distanceFromJunction,
    int32_t junctionTolerance)
{
    TileElement* tileElement;
    int32_t edges, slopeDirection;

    x += CoordsDirectionDelta[direction].x;
    y += CoordsDirectionDelta[direction].y;
    if (++level > 250)
        return FOOTPATH_SEARCH_TOO_COMPLEX;

    // Check if we are at edge of map
    if (x < 32 || y < 32)
        return FOOTPATH_SEARCH_SUCCESS;
    if (x >= gMapSizeUnits || y >= gMapSizeUnits)
        return FOOTPATH_SEARCH_SUCCESS;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tileElement->AsPath()->IsSloped() && (slopeDirection = tileElement->AsPath()->GetSlopeDirection()) != direction)
        {
            if (direction_reverse(slopeDirection) != direction)
                continue;
            if (tileElement->base_height + 2 != z)
                continue;
        }
        else if (tileElement->base_height != z)
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
            footpath_fix_ownership(x, y);
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
    z = tileElement->base_height;
    edges &= ~(1 << direction);

    // Find next direction to go
    if (!get_next_direction(edges, &direction))
    {
        return FOOTPATH_SEARCH_INCOMPLETE;
    }

    edges &= ~(1 << direction);
    if (edges == 0)
    {
        // Only possible direction to go
        if (tileElement->AsPath()->IsSloped() && tileElement->AsPath()->GetSlopeDirection() == direction)
        {
            z += 2;
        }
        return footpath_is_connected_to_map_edge_recurse(
            x, y, z, direction, flags, level, distanceFromJunction + 1, junctionTolerance);
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
            edges &= ~(1 << direction);
            if (tileElement->AsPath()->IsSloped() && tileElement->AsPath()->GetSlopeDirection() == direction)
            {
                z += 2;
            }
            int32_t result = footpath_is_connected_to_map_edge_recurse(x, y, z, direction, flags, level, 0, junctionTolerance);
            if (result == FOOTPATH_SEARCH_SUCCESS)
            {
                return result;
            }
        } while (get_next_direction(edges, &direction));

        return FOOTPATH_SEARCH_INCOMPLETE;
    }
}

int32_t footpath_is_connected_to_map_edge(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t flags)
{
    flags |= (1 << 0);
    return footpath_is_connected_to_map_edge_recurse(x, y, z, direction, flags, 0, 0, 16);
}

bool PathElement::IsSloped() const
{
    return (entryIndex & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED) != 0;
}

void PathElement::SetSloped(bool isSloped)
{
    entryIndex &= ~FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
    if (isSloped)
        entryIndex |= FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
}

uint8_t PathElement::GetSlopeDirection() const
{
    return entryIndex & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
}

void PathElement::SetSlopeDirection(uint8_t newSlope)
{
    entryIndex &= ~FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
    entryIndex |= newSlope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
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
    return (entryIndex & FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER) != 0;
}

void PathElement::SetHasQueueBanner(bool hasQueueBanner)
{
    entryIndex &= ~FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER;
    if (hasQueueBanner)
        entryIndex |= FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER;
}

bool PathElement::IsBroken() const
{
    return (flags & TILE_ELEMENT_FLAG_BROKEN) != 0;
}

void PathElement::SetIsBroken(bool isBroken)
{
    if (isBroken == true)
    {
        flags |= TILE_ELEMENT_FLAG_BROKEN;
    }
    else
    {
        flags &= ~TILE_ELEMENT_FLAG_BROKEN;
    }
}

bool PathElement::IsBlockedByVehicle() const
{
    return (flags & TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE) != 0;
}

void PathElement::SetIsBlockedByVehicle(bool isBlocked)
{
    if (isBlocked == true)
    {
        flags |= TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE;
    }
    else
    {
        flags &= ~TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE;
    }
}

uint8_t PathElement::GetStationIndex() const
{
    return (additions & FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK) >> 4;
}

void PathElement::SetStationIndex(uint8_t newStationIndex)
{
    additions &= ~FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK;
    additions |= ((newStationIndex << 4) & FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK);
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

bool PathElement::HasAddition() const
{
    return (additions & FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK) != 0;
}

uint8_t PathElement::GetAddition() const
{
    return additions & FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK;
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
    additions &= ~FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK;
    additions |= newAddition;
}

bool PathElement::AdditionIsGhost() const
{
    return (additions & FOOTPATH_ADDITION_FLAG_IS_GHOST) != 0;
}

void PathElement::SetAdditionIsGhost(bool isGhost)
{
    additions &= ~FOOTPATH_ADDITION_FLAG_IS_GHOST;
    if (isGhost)
        additions |= FOOTPATH_ADDITION_FLAG_IS_GHOST;
}

uint8_t PathElement::GetPathEntryIndex() const
{
    return (entryIndex & FOOTPATH_PROPERTIES_TYPE_MASK) >> 4;
}

uint8_t PathElement::GetRailingEntryIndex() const
{
    return GetPathEntryIndex();
}

PathSurfaceEntry* PathElement::GetPathEntry() const
{
    return get_path_surface_entry(GetPathEntryIndex());
}

PathRailingsEntry* PathElement::GetRailingEntry() const
{
    return get_path_railings_entry(GetRailingEntryIndex());
}

void PathElement::SetPathEntryIndex(uint8_t newEntryIndex)
{
    entryIndex &= ~FOOTPATH_PROPERTIES_TYPE_MASK;
    entryIndex |= (newEntryIndex << 4);
}

void PathElement::SetRailingEntryIndex(uint8_t newEntryIndex)
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
static void footpath_clear_wide(int32_t x, int32_t y)
{
    TileElement* tileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        tileElement->AsPath()->SetWide(false);
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006A8ACF
 *  returns footpath element if it can be made wide
 *  returns NULL if it can not be made wide
 */
static TileElement* footpath_can_be_wide(int32_t x, int32_t y, uint8_t height)
{
    TileElement* tileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (height != tileElement->base_height)
            continue;
        if (tileElement->AsPath()->IsQueue())
            continue;
        if (tileElement->AsPath()->IsSloped())
            continue;
        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 *
 *  rct2: 0x006A87BB
 */
void footpath_update_path_wide_flags(int32_t x, int32_t y)
{
    if (x < 0x20)
        return;
    if (y < 0x20)
        return;
    if (x > 0x1FDF)
        return;
    if (y > 0x1FDF)
        return;

    footpath_clear_wide(x, y);
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

    TileElement* tileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tileElement->AsPath()->IsQueue())
            continue;

        if (tileElement->AsPath()->IsSloped())
            continue;

        if (tileElement->AsPath()->GetEdges() == 0)
            continue;

        uint8_t height = tileElement->base_height;

        // pathList is a list of elements, set by sub_6A8ACF adjacent to x,y
        // Spanned from 0x00F3EFA8 to 0x00F3EFC7 (8 elements) in the original
        TileElement* pathList[8];

        x -= 0x20;
        y -= 0x20;
        pathList[0] = footpath_can_be_wide(x, y, height);
        y += 0x20;
        pathList[1] = footpath_can_be_wide(x, y, height);
        y += 0x20;
        pathList[2] = footpath_can_be_wide(x, y, height);
        x += 0x20;
        pathList[3] = footpath_can_be_wide(x, y, height);
        x += 0x20;
        pathList[4] = footpath_can_be_wide(x, y, height);
        y -= 0x20;
        pathList[5] = footpath_can_be_wide(x, y, height);
        y -= 0x20;
        pathList[6] = footpath_can_be_wide(x, y, height);
        x -= 0x20;
        pathList[7] = footpath_can_be_wide(x, y, height);
        y += 0x20;

        uint8_t pathConnections = 0;
        if (tileElement->AsPath()->GetEdges() & EDGE_NW)
        {
            pathConnections |= FOOTPATH_CONNECTION_NW;
            if (pathList[7] != nullptr && pathList[7]->AsPath()->IsWide())
            {
                pathConnections &= ~FOOTPATH_CONNECTION_NW;
            }
        }

        if (tileElement->AsPath()->GetEdges() & EDGE_NE)
        {
            pathConnections |= FOOTPATH_CONNECTION_NE;
            if (pathList[1] != nullptr && pathList[1]->AsPath()->IsWide())
            {
                pathConnections &= ~FOOTPATH_CONNECTION_NE;
            }
        }

        if (tileElement->AsPath()->GetEdges() & EDGE_SE)
        {
            pathConnections |= FOOTPATH_CONNECTION_SE;
            /* In the following:
             * footpath_element_is_wide(pathList[3])
             * is always false due to the tile update order
             * in combination with reset tiles.
             * Commented out since it will never occur. */
            // if (pathList[3] != nullptr) {
            //  if (footpath_element_is_wide(pathList[3])) {
            //      pathConnections &= ~FOOTPATH_CONNECTION_SE;
            //  }
            //}
        }

        if (tileElement->AsPath()->GetEdges() & EDGE_SW)
        {
            pathConnections |= FOOTPATH_CONNECTION_SW;
            /* In the following:
             * footpath_element_is_wide(pathList[5])
             * is always false due to the tile update order
             * in combination with reset tiles.
             * Commented out since it will never occur. */
            // if (pathList[5] != nullptr) {
            //  if (footpath_element_is_wide(pathList[5])) {
            //      pathConnections &= ~FOOTPATH_CONNECTION_SW;
            //  }
            //}
        }

        if ((pathConnections & FOOTPATH_CONNECTION_NW) && pathList[7] != nullptr && !pathList[7]->AsPath()->IsWide())
        {
            constexpr uint8_t edgeMask1 = EDGE_SE | EDGE_SW;
            if ((pathConnections & FOOTPATH_CONNECTION_NE) && pathList[0] != nullptr && !pathList[0]->AsPath()->IsWide()
                && (pathList[0]->AsPath()->GetEdges() & edgeMask1) == edgeMask1 && pathList[1] != nullptr
                && !pathList[1]->AsPath()->IsWide())
            {
                pathConnections |= FOOTPATH_CONNECTION_S;
            }

            /* In the following:
             * footpath_element_is_wide(pathList[5])
             * is always false due to the tile update order
             * in combination with reset tiles.
             * Short circuit the logic appropriately. */
            constexpr uint8_t edgeMask2 = EDGE_NE | EDGE_SE;
            if ((pathConnections & FOOTPATH_CONNECTION_SW) && pathList[6] != nullptr && !(pathList[6])->AsPath()->IsWide()
                && (pathList[6]->AsPath()->GetEdges() & edgeMask2) == edgeMask2 && pathList[5] != nullptr)
            {
                pathConnections |= FOOTPATH_CONNECTION_E;
            }
        }

        /* In the following:
         * footpath_element_is_wide(pathList[2])
         * footpath_element_is_wide(pathList[3])
         * are always false due to the tile update order
         * in combination with reset tiles.
         * Short circuit the logic appropriately. */
        if ((pathConnections & FOOTPATH_CONNECTION_SE) && pathList[3] != nullptr)
        {
            constexpr uint8_t edgeMask1 = EDGE_SW | EDGE_NW;
            if ((pathConnections & FOOTPATH_CONNECTION_NE) && (pathList[2] != nullptr)
                && (pathList[2]->AsPath()->GetEdges() & edgeMask1) == edgeMask1 && pathList[1] != nullptr
                && !pathList[1]->AsPath()->IsWide())
            {
                pathConnections |= FOOTPATH_CONNECTION_W;
            }

            /* In the following:
             * footpath_element_is_wide(pathList[4])
             * footpath_element_is_wide(pathList[5])
             * are always false due to the tile update order
             * in combination with reset tiles.
             * Short circuit the logic appropriately. */
            constexpr uint8_t edgeMask2 = EDGE_NE | EDGE_NW;
            if ((pathConnections & FOOTPATH_CONNECTION_SW) && pathList[4] != nullptr
                && (pathList[4]->AsPath()->GetEdges() & edgeMask2) == edgeMask2 && pathList[5] != nullptr)
            {
                pathConnections |= FOOTPATH_CONNECTION_N;
            }
        }

        if ((pathConnections & FOOTPATH_CONNECTION_NW) && (pathConnections & (FOOTPATH_CONNECTION_E | FOOTPATH_CONNECTION_S)))
        {
            pathConnections &= ~FOOTPATH_CONNECTION_NW;
        }

        if ((pathConnections & FOOTPATH_CONNECTION_NE) && (pathConnections & (FOOTPATH_CONNECTION_W | FOOTPATH_CONNECTION_S)))
        {
            pathConnections &= ~FOOTPATH_CONNECTION_NE;
        }

        if ((pathConnections & FOOTPATH_CONNECTION_SE) && (pathConnections & (FOOTPATH_CONNECTION_N | FOOTPATH_CONNECTION_W)))
        {
            pathConnections &= ~FOOTPATH_CONNECTION_SE;
        }

        if ((pathConnections & FOOTPATH_CONNECTION_SW) && (pathConnections & (FOOTPATH_CONNECTION_E | FOOTPATH_CONNECTION_N)))
        {
            pathConnections &= ~FOOTPATH_CONNECTION_SW;
        }

        if (!(pathConnections
              & (FOOTPATH_CONNECTION_NE | FOOTPATH_CONNECTION_SE | FOOTPATH_CONNECTION_SW | FOOTPATH_CONNECTION_NW)))
        {
            uint8_t e = tileElement->AsPath()->GetEdgesAndCorners();
            if ((e != 0b10101111) && (e != 0b01011111) && (e != 0b11101111))
                tileElement->AsPath()->SetWide(true);
        }
    } while (!(tileElement++)->IsLastForTile());
}

bool footpath_is_blocked_by_vehicle(const TileCoordsXYZ& position)
{
    auto pathElement = map_get_path_element_at(position.x, position.y, position.z);
    return pathElement != nullptr && pathElement->AsPath()->IsBlockedByVehicle();
}

/**
 *
 *  rct2: 0x006A7642
 */
void footpath_update_queue_entrance_banner(int32_t x, int32_t y, TileElement* tileElement)
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
                        footpath_chain_ride_queue(255, 0, x, y, tileElement, direction);
                    }
                }
                tileElement->AsPath()->SetRideIndex(RIDE_ID_NULL);
            }
            break;
        case TILE_ELEMENT_TYPE_ENTRANCE:
            if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
            {
                footpath_queue_chain_push(tileElement->AsEntrance()->GetRideIndex());
                footpath_chain_ride_queue(255, 0, x, y, tileElement, tileElement->GetDirectionWithOffset(2));
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006A6B7F
 */
static void footpath_remove_edges_towards_here(
    int32_t x, int32_t y, int32_t z, int32_t direction, TileElement* tileElement, bool isQueue)
{
    if (tileElement->AsPath()->IsQueue())
    {
        footpath_queue_chain_push(tileElement->AsPath()->GetRideIndex());
    }

    int32_t d = direction_reverse(direction);
    tileElement->AsPath()->SetEdges(tileElement->AsPath()->GetEdges() & ~(1 << d));
    int32_t cd = ((d - 1) & 3);
    tileElement->AsPath()->SetCorners(tileElement->AsPath()->GetCorners() & ~(1 << cd));
    cd = ((cd + 1) & 3);
    tileElement->AsPath()->SetCorners(tileElement->AsPath()->GetCorners() & ~(1 << cd));
    map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);

    if (isQueue)
        footpath_disconnect_queue_from_path(x, y, tileElement, -1);

    direction = (direction + 1) & 3;
    x += CoordsDirectionDelta[direction].x;
    y += CoordsDirectionDelta[direction].y;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (tileElement->base_height != z)
            continue;

        if (tileElement->AsPath()->IsSloped())
            break;

        cd = ((direction + 1) & 3);
        tileElement->AsPath()->SetCorners(tileElement->AsPath()->GetCorners() & ~(1 << cd));
        map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
        break;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006A6B14
 */
static void footpath_remove_edges_towards(int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t direction, bool isQueue)
{
    if (!map_is_location_valid({ x, y }))
    {
        return;
    }

    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (z1 == tileElement->base_height)
        {
            if (tileElement->AsPath()->IsSloped())
            {
                uint8_t slope = tileElement->AsPath()->GetSlopeDirection();
                if (slope != direction)
                    break;
            }
            footpath_remove_edges_towards_here(x, y, z1, direction, tileElement, isQueue);
            break;
        }

        if (z0 == tileElement->base_height)
        {
            if (!tileElement->AsPath()->IsSloped())
                break;

            uint8_t slope = direction_reverse(tileElement->AsPath()->GetSlopeDirection());
            if (slope != direction)
                break;

            footpath_remove_edges_towards_here(x, y, z1, direction, tileElement, isQueue);
            break;
        }
    } while (!(tileElement++)->IsLastForTile());
}

// Returns true when there is an element at the given coordinates that want to connect to a path with the given direction (ride
// entrances and exits, shops, paths).
bool tile_element_wants_path_connection_towards(TileCoordsXYZD coords, const TileElement* const elementToBeRemoved)
{
    TileElement* tileElement = map_get_first_element_at(coords.x, coords.y);
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
                    Ride* ride = get_ride(tileElement->AsTrack()->GetRideIndex());
                    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                        break;

                    const uint8_t trackType = tileElement->AsTrack()->GetTrackType();
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
static void footpath_fix_corners_around(int32_t x, int32_t y, TileElement* pathElement)
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

            TileElement* tileElement = map_get_first_element_at(x + xOffset, y + yOffset);
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
void footpath_remove_edges_at(int32_t x, int32_t y, TileElement* tileElement)
{
    if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
    {
        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
        Ride* ride = get_ride(rideIndex);
        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            return;
    }

    footpath_update_queue_entrance_banner(x, y, tileElement);

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
        if (!tile_element_wants_path_connection_towards({ x / 32, y / 32, z1, direction }, tileElement))
        {
            bool isQueue = tileElement->GetType() == TILE_ELEMENT_TYPE_PATH ? tileElement->AsPath()->IsQueue() : false;
            int32_t z0 = z1 - 2;
            footpath_remove_edges_towards(
                x + CoordsDirectionDelta[direction].x, y + CoordsDirectionDelta[direction].y, z0, z1, direction, isQueue);
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
        footpath_fix_corners_around(x / 32, y / 32, tileElement);
    }

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        tileElement->AsPath()->SetEdgesAndCorners(0);
}

PathSurfaceEntry* get_path_surface_entry(int32_t entryIndex)
{
    PathSurfaceEntry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_PATHS, entryIndex);
    if (obj != nullptr)
    {
        result = ((FootpathObject*)obj)->GetPathSurfaceEntry();
    }
    return result;
}

PathRailingsEntry* get_path_railings_entry(int32_t entryIndex)
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
    return additionStatus;
}

void PathElement::SetAdditionStatus(uint8_t newStatus)
{
    additionStatus = newStatus;
}

uint8_t PathElement::GetEdges() const
{
    return edges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
}

void PathElement::SetEdges(uint8_t newEdges)
{
    edges &= ~FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
    edges |= (newEdges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK);
}

uint8_t PathElement::GetCorners() const
{
    return edges >> 4;
}

void PathElement::SetCorners(uint8_t newCorners)
{
    edges &= ~FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK;
    edges |= (newCorners << 4);
}

uint8_t PathElement::GetEdgesAndCorners() const
{
    return edges;
}

void PathElement::SetEdgesAndCorners(uint8_t newEdgesAndCorners)
{
    edges = newEdgesAndCorners;
}
