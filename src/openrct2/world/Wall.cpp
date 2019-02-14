/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Wall.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../common.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../ride/RideGroupManager.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "Banner.h"
#include "LargeScenery.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"
#include "SmallScenery.h"
#include "Surface.h"
#include "Wall.h"

/**
 * Gets whether the given track type can have a wall placed on the edge of the given direction.
 * Some thin tracks for example are allowed to have walls either side of the track, but wider tracks can not.
 */
static bool TrackIsAllowedWallEdges(uint8_t rideType, uint8_t trackType, uint8_t trackSequence, uint8_t direction)
{
    if (!ride_type_has_flag(rideType, RIDE_TYPE_FLAG_TRACK_NO_WALLS))
    {
        if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            if (FlatRideTrackSequenceElementAllowedWallEdges[trackType][trackSequence] & (1 << direction))
            {
                return true;
            }
        }
        else
        {
            if (TrackSequenceElementAllowedWallEdges[trackType][trackSequence] & (1 << direction))
            {
                return true;
            }
        }
    }
    return false;
}

/**
 *
 *  rct2: 0x006E5CBA
 */
static bool WallCheckObstructionWithTrack(
    rct_scenery_entry* wall, int32_t z0, int32_t edge, TileElement* trackElement, bool* wallAcrossTrack)
{
    int32_t trackType = trackElement->AsTrack()->GetTrackType();
    int32_t sequence = trackElement->AsTrack()->GetSequenceIndex();
    int32_t direction = (edge - trackElement->GetDirection()) & TILE_ELEMENT_DIRECTION_MASK;
    Ride* ride = get_ride(trackElement->AsTrack()->GetRideIndex());

    if (TrackIsAllowedWallEdges(ride->type, trackType, sequence, direction))
    {
        return true;
    }

    if (!(wall->wall.flags & WALL_SCENERY_IS_DOOR))
    {
        return false;
    }

    if (RideGroupManager::RideTypeHasRideGroups(ride->type))
    {
        auto rideGroup = RideGroupManager::GetRideGroup(ride->type, get_ride_entry(ride->subtype));
        if (!(rideGroup->Flags & RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK))
        {
            return false;
        }
    }
    else if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_DOORS_ON_TRACK))
    {
        return false;
    }

    *wallAcrossTrack = true;
    if (z0 & 1)
    {
        return false;
    }

    int32_t z;
    if (sequence == 0)
    {
        if (TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_DISALLOW_DOORS)
        {
            return false;
        }

        if (TrackDefinitions[trackType].bank_start == 0)
        {
            if (!(TrackCoordinates[trackType].rotation_begin & 4))
            {
                direction = trackElement->GetDirectionWithOffset(2);
                if (direction == edge)
                {
                    const rct_preview_track* trackBlock = &TrackBlocks[trackType][sequence];
                    z = TrackCoordinates[trackType].z_begin;
                    z = trackElement->base_height + ((z - trackBlock->z) * 8);
                    if (z == z0)
                    {
                        return true;
                    }
                }
            }
        }
    }

    const rct_preview_track* trackBlock = &TrackBlocks[trackType][sequence + 1];
    if (trackBlock->index != 0xFF)
    {
        return false;
    }

    if (TrackDefinitions[trackType].bank_end != 0)
    {
        return false;
    }

    direction = TrackCoordinates[trackType].rotation_end;
    if (direction & 4)
    {
        return false;
    }

    direction = trackElement->GetDirection();
    if (direction != edge)
    {
        return false;
    }

    trackBlock = &TrackBlocks[trackType][sequence];
    z = TrackCoordinates[trackType].z_end;
    z = trackElement->base_height + ((z - trackBlock->z) * 8);
    return z == z0;
}

/**
 *
 *  rct2: 0x006E5C1A
 */
static bool WallCheckObstruction(
    rct_scenery_entry* wall, int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t edge, bool* wallAcrossTrack)
{
    int32_t entryType, sequence;
    rct_scenery_entry* entry;
    rct_large_scenery_tile* tile;

    *wallAcrossTrack = false;
    gMapGroundFlags = ELEMENT_IS_ABOVE_GROUND;
    if (map_is_location_at_edge(x, y))
    {
        gGameCommandErrorText = STR_OFF_EDGE_OF_MAP;
        return false;
    }

    TileElement* tileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        int32_t elementType = tileElement->GetType();
        if (elementType == TILE_ELEMENT_TYPE_SURFACE)
            continue;
        if (z0 >= tileElement->clearance_height)
            continue;
        if (z1 <= tileElement->base_height)
            continue;
        if (elementType == TILE_ELEMENT_TYPE_WALL)
        {
            int32_t direction = tileElement->GetDirection();
            if (edge == direction)
            {
                map_obstruction_set_error_text(tileElement);
                return false;
            }
            continue;
        }
        if ((tileElement->flags & 0x0F) == 0)
            continue;

        switch (elementType)
        {
            case TILE_ELEMENT_TYPE_ENTRANCE:
                map_obstruction_set_error_text(tileElement);
                return false;
            case TILE_ELEMENT_TYPE_PATH:
                if (tileElement->AsPath()->GetEdges() & (1 << edge))
                {
                    map_obstruction_set_error_text(tileElement);
                    return false;
                }
                break;
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                entryType = tileElement->AsLargeScenery()->GetEntryIndex();
                sequence = tileElement->AsLargeScenery()->GetSequenceIndex();
                entry = get_large_scenery_entry(entryType);
                tile = &entry->large_scenery.tiles[sequence];
                {
                    int32_t direction = ((edge - tileElement->GetDirection()) & TILE_ELEMENT_DIRECTION_MASK) + 8;
                    if (!(tile->flags & (1 << direction)))
                    {
                        map_obstruction_set_error_text(tileElement);
                        return false;
                    }
                }
                break;
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                entry = tileElement->AsSmallScenery()->GetEntry();
                if (scenery_small_entry_has_flag(entry, SMALL_SCENERY_FLAG_NO_WALLS))
                {
                    map_obstruction_set_error_text(tileElement);
                    return false;
                }
                break;
            case TILE_ELEMENT_TYPE_TRACK:
                if (!WallCheckObstructionWithTrack(wall, z0, edge, tileElement, wallAcrossTrack))
                {
                    return false;
                }
                break;
        }
    } while (!(tileElement++)->IsLastForTile());

    return true;
}

#pragma region Edge Slopes Table

// clang-format off
enum EDGE_SLOPE
{
    EDGE_SLOPE_ELEVATED     = (1 << 0), // 0x01
    EDGE_SLOPE_UPWARDS      = (1 << 6), // 0x40
    EDGE_SLOPE_DOWNWARDS    = (1 << 7), // 0x80

    EDGE_SLOPE_UPWARDS_ELEVATED     = EDGE_SLOPE_UPWARDS | EDGE_SLOPE_ELEVATED,
    EDGE_SLOPE_DOWNWARDS_ELEVATED   = EDGE_SLOPE_DOWNWARDS | EDGE_SLOPE_ELEVATED,
};

/** rct2: 0x009A3FEC */
static constexpr const uint8_t EdgeSlopes[][4] = {
//  Top right                        Bottom right                   Bottom left                       Top left
    { 0,                             0,                             0,                             0                             },
    { 0,                             EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS,          0                             },
    { 0,                             0,                             EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS          },
    { 0,                             EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS          },
    { EDGE_SLOPE_DOWNWARDS,          0,                             0,                             EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_DOWNWARDS,          0,                             EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED           },
    { EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED           },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS,          0,                             0                             },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS,          0                             },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS          },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS          },
    { EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS,          0,                             EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED           },
    { EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED           },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_UPWARDS_ELEVATED,   EDGE_SLOPE_DOWNWARDS_ELEVATED },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_UPWARDS_ELEVATED,   EDGE_SLOPE_DOWNWARDS_ELEVATED, EDGE_SLOPE_DOWNWARDS          },
    { 0,                             0,                             0,                             0                             },
    { EDGE_SLOPE_UPWARDS_ELEVATED,   EDGE_SLOPE_DOWNWARDS_ELEVATED, EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_DOWNWARDS_ELEVATED, EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_UPWARDS_ELEVATED   },
    { 0,                             0,                             0,                             0                             },
};
// clang-format on

#pragma endregion

static money32 WallPlace(
    uint8_t wallType, int16_t x, int16_t y, int16_t z, uint8_t edge, uint8_t primaryColour, uint8_t secondaryColour,
    uint8_t tertiaryColour, uint8_t flags)
{
    LocationXYZ16 position = { x, y, z };

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    gCommandPosition.x = position.x + 16;
    gCommandPosition.y = position.y + 16;
    gCommandPosition.z = position.z;

    if (position.z == 0)
    {
        gCommandPosition.z = tile_element_height(position.x, position.y) & 0xFFFF;
    }

    if (game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(flags & GAME_COMMAND_FLAG_PATH_SCENERY) && !gCheatsSandboxMode)
    {
        if (position.z == 0)
        {
            if (!map_is_location_in_park({ position.x, position.y }))
            {
                return MONEY32_UNDEFINED;
            }
        }
        else if (!map_is_location_owned(position.x, position.y, position.z))
        {
            return MONEY32_UNDEFINED;
        }
    }

    uint8_t edgeSlope = 0;
    if (position.z == 0)
    {
        TileElement* surfaceElement = map_get_surface_element_at({ position.x, position.y });
        if (surfaceElement == nullptr)
        {
            return MONEY32_UNDEFINED;
        }
        position.z = surfaceElement->base_height * 8;

        uint8_t slope = surfaceElement->AsSurface()->GetSlope();
        edgeSlope = EdgeSlopes[slope][edge & 3];
        if (edgeSlope & EDGE_SLOPE_ELEVATED)
        {
            position.z += 16;
            edgeSlope &= ~EDGE_SLOPE_ELEVATED;
        }
    }

    TileElement* surfaceElement = map_get_surface_element_at({ position.x, position.y });
    if (surfaceElement == nullptr)
    {
        return MONEY32_UNDEFINED;
    }

    if (surfaceElement->AsSurface()->GetWaterHeight() > 0)
    {
        uint16_t waterHeight = surfaceElement->AsSurface()->GetWaterHeight() * 16;

        if (position.z < waterHeight && !gCheatsDisableClearanceChecks)
        {
            gGameCommandErrorText = STR_CANT_BUILD_THIS_UNDERWATER;
            return MONEY32_UNDEFINED;
        }
    }

    if (position.z / 8 < surfaceElement->base_height && !gCheatsDisableClearanceChecks)
    {
        gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
        return MONEY32_UNDEFINED;
    }

    if (!(edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS)))
    {
        uint8_t newEdge = (edge + 2) & 3;
        uint8_t newBaseHeight = surfaceElement->base_height;
        newBaseHeight += 2;
        if (surfaceElement->AsSurface()->GetSlope() & (1 << newEdge))
        {
            if (position.z / 8 < newBaseHeight)
            {
                gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                return MONEY32_UNDEFINED;
            }

            if (surfaceElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            {
                newEdge = (newEdge - 1) & 3;

                if (surfaceElement->AsSurface()->GetSlope() & (1 << newEdge))
                {
                    newEdge = (newEdge + 2) & 3;
                    if (surfaceElement->AsSurface()->GetSlope() & (1 << newEdge))
                    {
                        newBaseHeight += 2;
                        if (position.z / 8 < newBaseHeight)
                        {
                            gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                            return MONEY32_UNDEFINED;
                        }
                        newBaseHeight -= 2;
                    }
                }
            }
        }

        newEdge = (edge + 3) & 3;
        if (surfaceElement->AsSurface()->GetSlope() & (1 << newEdge))
        {
            if (position.z / 8 < newBaseHeight)
            {
                gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                return MONEY32_UNDEFINED;
            }

            if (surfaceElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            {
                newEdge = (newEdge - 1) & 3;

                if (surfaceElement->AsSurface()->GetSlope() & (1 << newEdge))
                {
                    newEdge = (newEdge + 2) & 3;
                    if (surfaceElement->AsSurface()->GetSlope() & (1 << newEdge))
                    {
                        newBaseHeight += 2;
                        if (position.z / 8 < newBaseHeight)
                        {
                            gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                            return MONEY32_UNDEFINED;
                        }
                    }
                }
            }
        }
    }
    BannerIndex bannerIndex = BANNER_INDEX_NULL;
    rct_scenery_entry* wallEntry = get_wall_entry(wallType);

    if (wallEntry == nullptr)
    {
        return MONEY32_UNDEFINED;
    }

    if (wallEntry->wall.scrolling_mode != SCROLLING_MODE_NONE)
    {
        bannerIndex = create_new_banner(flags);

        if (bannerIndex == 0xFF)
        {
            return MONEY32_UNDEFINED;
        }

        rct_banner* banner = &gBanners[bannerIndex];
        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            banner->flags |= BANNER_FLAG_IS_WALL;
            banner->type = 0;
            banner->x = position.x / 32;
            banner->y = position.y / 32;

            ride_id_t rideIndex = banner_get_closest_ride_index(position.x, position.y, position.z);
            if (rideIndex != RIDE_ID_NULL)
            {
                banner->ride_index = rideIndex;
                banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;
            }
        }
    }

    uint8_t clearanceHeight = position.z / 8;
    if (edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS))
    {
        if (wallEntry->wall.flags & WALL_SCENERY_CANT_BUILD_ON_SLOPE)
        {
            gGameCommandErrorText = STR_ERR_UNABLE_TO_BUILD_THIS_ON_SLOPE;
            return MONEY32_UNDEFINED;
        }
        clearanceHeight += 2;
    }
    clearanceHeight += wallEntry->wall.height;

    bool wallAcrossTrack = false;
    if (!(flags & GAME_COMMAND_FLAG_PATH_SCENERY) && !gCheatsDisableClearanceChecks)
    {
        if (!WallCheckObstruction(wallEntry, position.x, position.y, position.z / 8, clearanceHeight, edge, &wallAcrossTrack))
        {
            return MONEY32_UNDEFINED;
        }
    }

    if (!map_check_free_elements_and_reorganise(1))
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            LocationXYZ16 coord;
            coord.x = position.x + 16;
            coord.y = position.y + 16;
            coord.z = tile_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        TileElement* tileElement = tile_element_insert(position.x / 32, position.y / 32, position.z / 8, 0);
        assert(tileElement != nullptr);

        map_animation_create(MAP_ANIMATION_TYPE_WALL, position.x, position.y, position.z / 8);

        tileElement->clearance_height = clearanceHeight;
        tileElement->SetType(TILE_ELEMENT_TYPE_WALL);
        tileElement->SetDirection(edge);
        // TODO: Normalise the edge slope code.
        tileElement->AsWall()->SetSlope(edgeSlope >> 6);

        tileElement->AsWall()->SetPrimaryColour(primaryColour);
        tileElement->AsWall()->SetSecondaryColour(secondaryColour);

        if (wallAcrossTrack)
        {
            tileElement->AsWall()->SetAcrossTrack(true);
        }

        tileElement->AsWall()->SetEntryIndex(wallType);
        if (bannerIndex != 0xFF)
        {
            tileElement->AsWall()->SetBannerIndex(bannerIndex);
        }

        if (wallEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
        {
            tileElement->AsWall()->SetTertiaryColour(tertiaryColour);
        }

        if (flags & GAME_COMMAND_FLAG_GHOST)
        {
            tileElement->flags |= TILE_ELEMENT_FLAG_GHOST;
        }

        gSceneryTileElement = tileElement;
        map_invalidate_tile_zoom1(position.x, position.y, tileElement->base_height * 8, tileElement->base_height * 8 + 72);
    }

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return 0;
    }
    else
    {
        return wallEntry->wall.price;
    }
}

static money32 WallSetColour(
    int16_t x, int16_t y, uint8_t baseHeight, uint8_t direction, uint8_t primaryColour, uint8_t secondaryColour,
    uint8_t tertiaryColour, uint8_t flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    int32_t z = baseHeight * 8;

    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !map_is_location_in_park({ x, y }) && !gCheatsSandboxMode)
    {
        return MONEY32_UNDEFINED;
    }

    TileElement* wallElement = map_get_wall_element_at(x, y, baseHeight, direction);
    if (wallElement == nullptr)
    {
        return 0;
    }

    if ((flags & GAME_COMMAND_FLAG_GHOST) && !(wallElement->flags & TILE_ELEMENT_FLAG_GHOST))
    {
        return 0;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        rct_scenery_entry* scenery_entry = wallElement->AsWall()->GetEntry();
        wallElement->AsWall()->SetPrimaryColour(primaryColour);
        wallElement->AsWall()->SetSecondaryColour(secondaryColour);

        if (scenery_entry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
        {
            wallElement->AsWall()->SetTertiaryColour(tertiaryColour);
        }
        map_invalidate_tile_zoom1(x, y, z, z + 72);
    }

    return 0;
}

/**
 *
 *  rct2: 0x006E588E
 */
void wall_remove_at(int32_t x, int32_t y, int32_t z0, int32_t z1)
{
    TileElement* tileElement;

    z0 /= 8;
    z1 /= 8;
repeat:
    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (z0 >= tileElement->clearance_height)
            continue;
        if (z1 <= tileElement->base_height)
            continue;

        tile_element_remove_banner_entry(tileElement);
        map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->base_height * 8 + 72);
        tile_element_remove(tileElement);
        goto repeat;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006E57E6
 */
void wall_remove_at_z(int32_t x, int32_t y, int32_t z)
{
    wall_remove_at(x, y, z, z + 48);
}

/**
 *
 *  rct2: 0x006E5935
 */
void wall_remove_intersecting_walls(int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t direction)
{
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;

        if (tileElement->clearance_height <= z0 || tileElement->base_height >= z1)
            continue;

        if (direction != tileElement->GetDirection())
            continue;

        tile_element_remove_banner_entry(tileElement);
        map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->base_height * 8 + 72);
        tile_element_remove(tileElement);
        tileElement--;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006E519A
 */
void game_command_place_wall(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    *ebx = WallPlace(
        (*ebx >> 8) & 0xFF, *eax & 0xFFFF, *ecx & 0xFFFF, *edi & 0xFFFF, *edx & 0xFF, (*edx >> 8) & 0xFF, *ebp & 0xFF,
        (*ebp >> 8) & 0xFF, *ebx & 0xFF);
}

money32 wall_place(
    int32_t type, int32_t x, int32_t y, int32_t z, int32_t edge, int32_t primaryColour, int32_t secondaryColour,
    int32_t tertiaryColour, int32_t flags)
{
    int32_t eax = x;
    int32_t ebx = flags | (type << 8);
    int32_t ecx = y;
    int32_t edx = edge | (primaryColour << 8);
    int32_t esi = 0;
    int32_t edi = z;
    int32_t ebp = secondaryColour | (tertiaryColour << 8);
    game_command_place_wall(&eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
    return ebx;
}

/**
 *
 *  rct2: 0x006E56B5
 */
void game_command_set_wall_colour(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, [[maybe_unused]] int32_t* edi,
    int32_t* ebp)
{
    *ebx = WallSetColour(
        *eax & 0xFFFF, *ecx & 0xFFFF, (*edx >> 8) & 0xFF, *edx & 0xFF, (*ebx >> 8) & 0xFF, *ebp & 0xFF, (*ebp >> 8) & 0xFF,
        *ebx & 0xFF);
}

uint8_t WallElement::GetSlope() const
{
    return (type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
}

void WallElement::SetSlope(uint8_t newSlope)
{
    type &= ~TILE_ELEMENT_QUADRANT_MASK;
    type |= (newSlope << 6);
}

colour_t WallElement::GetPrimaryColour() const
{
    return colour_1 & TILE_ELEMENT_COLOUR_MASK;
}

colour_t WallElement::GetSecondaryColour() const
{
    uint8_t secondaryColour = (colour_1 & ~TILE_ELEMENT_COLOUR_MASK) >> 5;
    secondaryColour |= (flags & 0x60) >> 2;
    return secondaryColour;
}

colour_t WallElement::GetTertiaryColour() const
{
    return colour_3 & TILE_ELEMENT_COLOUR_MASK;
}

void WallElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour_1 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= newColour;
}

void WallElement::SetSecondaryColour(colour_t newColour)
{
    colour_1 &= TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= (newColour & 0x7) << 5;
    flags &= ~0x60;
    flags |= (newColour & 0x18) << 2;
}

void WallElement::SetTertiaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour_3 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_3 |= newColour;
}

uint8_t WallElement::GetAnimationFrame() const
{
    return (animation >> 3) & 0xF;
}

void WallElement::SetAnimationFrame(uint8_t frameNum)
{
    animation &= WALL_ANIMATION_FLAG_ALL_FLAGS;
    animation |= (frameNum & 0xF) << 3;
}

uint8_t WallElement::GetEntryIndex() const
{
    return entryIndex;
}

rct_scenery_entry* WallElement::GetEntry() const
{
    return get_wall_entry(entryIndex);
}

void WallElement::SetEntryIndex(uint8_t newIndex)
{
    entryIndex = newIndex;
}

BannerIndex WallElement::GetBannerIndex() const
{
    return banner_index;
}

void WallElement::SetBannerIndex(BannerIndex newIndex)
{
    banner_index = newIndex;
}

bool WallElement::IsAcrossTrack() const
{
    return (animation & WALL_ANIMATION_FLAG_ACROSS_TRACK) != 0;
}

void WallElement::SetAcrossTrack(bool acrossTrack)
{
    animation &= ~WALL_ANIMATION_FLAG_ACROSS_TRACK;
    if (acrossTrack)
        animation |= WALL_ANIMATION_FLAG_ACROSS_TRACK;
}

bool WallElement::AnimationIsBackwards() const
{
    return (animation & WALL_ANIMATION_FLAG_DIRECTION_BACKWARD) != 0;
}

void WallElement::SetAnimationIsBackwards(bool isBackwards)
{
    animation &= ~WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
    if (isBackwards)
        animation |= WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
}

void WallElement::SetRawRCT1Data(uint32_t rawData)
{
    colour_3 = rawData & 0xFF;
    colour_1 = (rawData >> 8) & 0xFF;
    animation = (rawData >> 16) & 0xFF;
}
