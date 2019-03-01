/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Scenery.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../actions/LargeSceneryRemoveAction.hpp"
#include "../actions/SmallSceneryRemoveAction.hpp"
#include "../actions/FootpathSceneryRemoveAction.hpp"
#include "../actions/WallRemoveAction.hpp"
#include "../common.h"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../scenario/Scenario.h"
#include "Climate.h"
#include "Footpath.h"
#include "Fountain.h"
#include "Map.h"
#include "Park.h"
#include "SmallScenery.h"
#include "Wall.h"

uint8_t gWindowSceneryActiveTabIndex;
uint16_t gWindowSceneryTabSelections[20];
uint8_t gWindowSceneryClusterEnabled;
uint8_t gWindowSceneryPaintEnabled;
uint8_t gWindowSceneryRotation;
colour_t gWindowSceneryPrimaryColour;
colour_t gWindowScenerySecondaryColour;
colour_t gWindowSceneryTertiaryColour;
bool gWindowSceneryEyedropperEnabled;

TileElement* gSceneryTileElement;
uint8_t gSceneryQuadrant;

money32 gSceneryPlaceCost;
int16_t gSceneryPlaceObject;
int16_t gSceneryPlaceZ;
uint8_t gSceneryPlacePathType;
uint8_t gSceneryPlacePathSlope;
uint8_t gSceneryPlaceRotation;

uint8_t gSceneryGhostType;
LocationXYZ16 gSceneryGhostPosition;
uint32_t gSceneryGhostPathObjectType;
uint8_t gSceneryGhostWallRotation;

int16_t gSceneryShiftPressed;
int16_t gSceneryShiftPressX;
int16_t gSceneryShiftPressY;
int16_t gSceneryShiftPressZOffset;

int16_t gSceneryCtrlPressed;
int16_t gSceneryCtrlPressZ;

uint8_t gSceneryGroundFlags;

money32 gClearSceneryCost;

// rct2: 0x009A3E74
const LocationXY8 ScenerySubTileOffsets[] = { { 7, 7 }, { 7, 23 }, { 23, 23 }, { 23, 7 } };

void scenery_update_tile(int32_t x, int32_t y)
{
    TileElement* tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_SMALL_SCENERY)
        {
            scenery_update_age(x, y, tileElement);
        }
        else if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (tileElement->AsPath()->HasAddition() && !tileElement->AsPath()->AdditionIsGhost())
            {
                rct_scenery_entry* sceneryEntry = tileElement->AsPath()->GetAdditionEntry();
                if (sceneryEntry != nullptr)
                {
                    if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER)
                    {
                        jumping_fountain_begin(JUMPING_FOUNTAIN_TYPE_WATER, x, y, tileElement);
                    }
                    else if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW)
                    {
                        jumping_fountain_begin(JUMPING_FOUNTAIN_TYPE_SNOW, x, y, tileElement);
                    }
                }
            }
        }
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006E33D9
 */
void scenery_update_age(int32_t x, int32_t y, TileElement* tileElement)
{
    TileElement* tileElementAbove;
    rct_scenery_entry* sceneryEntry;

    sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
    if (sceneryEntry == nullptr)
    {
        return;
    }

    if (gCheatsDisablePlantAging && (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_CAN_BE_WATERED)))
    {
        return;
    }

    if (!scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_CAN_BE_WATERED)
        || (gClimateCurrent.Weather < WEATHER_RAIN) || (tileElement->AsSmallScenery()->GetAge() < 5))
    {
        tileElement->AsSmallScenery()->IncreaseAge(x, y);
        return;
    }

    // Check map elements above, presumably to see if map element is blocked from rain
    tileElementAbove = tileElement;
    while (!(tileElementAbove->flags & 7))
    {
        tileElementAbove++;

        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (tileElementAbove->IsGhost())
            continue;

        switch (tileElementAbove->GetType())
        {
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            case TILE_ELEMENT_TYPE_ENTRANCE:
            case TILE_ELEMENT_TYPE_PATH:
                map_invalidate_tile_zoom1(x, y, tileElementAbove->base_height * 8, tileElementAbove->clearance_height * 8);
                tileElement->AsSmallScenery()->IncreaseAge(x, y);
                return;
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                sceneryEntry = tileElementAbove->AsSmallScenery()->GetEntry();
                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
                {
                    tileElement->AsSmallScenery()->IncreaseAge(x, y);
                    return;
                }
                break;
        }
    }

    // Reset age / water plant
    tileElement->AsSmallScenery()->SetAge(0);
    map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
}

/**
 *
 *  rct2: 0x006E2712
 */
void scenery_remove_ghost_tool_placement()
{
    int16_t x, y, z;

    x = gSceneryGhostPosition.x;
    y = gSceneryGhostPosition.y;
    z = gSceneryGhostPosition.z;

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_0)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_0;

        auto removeSceneryAction = SmallSceneryRemoveAction(x, y, z, gSceneryQuadrant, gSceneryPlaceObject);
        removeSceneryAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_GHOST);
        removeSceneryAction.Execute();
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_1)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_1;
        TileElement* tileElement = map_get_first_element_at(x / 32, y / 32);

        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
                continue;

            if (tileElement->base_height != z)
                continue;

            auto footpathSceneryRemoveAction = FootpathSceneryRemoveAction({x, y, z * 8});
            footpathSceneryRemoveAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST);
            GameActions::Execute(&footpathSceneryRemoveAction);
            break;
        } while (!(tileElement++)->IsLastForTile());
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_2)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_2;

        TileCoordsXYZD wallLocation = { x >> 5, y >> 5, z, gSceneryGhostWallRotation };
        auto wallRemoveAction = WallRemoveAction(wallLocation);
        wallRemoveAction.SetFlags(GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_PATH_SCENERY);
        wallRemoveAction.Execute();
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_3)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_3;

        auto removeSceneryAction = LargeSceneryRemoveAction(x, y, z, gSceneryPlaceRotation, 0);
        removeSceneryAction.SetFlags(
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5);
        removeSceneryAction.Execute();
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_4)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_4;
        constexpr uint32_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
            | GAME_COMMAND_FLAG_5;
        game_do_command(x, flags, y, z | (gSceneryPlaceRotation << 8), GAME_COMMAND_REMOVE_BANNER, 0, 0);
    }
}

rct_scenery_entry* get_wall_entry(int32_t entryIndex)
{
    rct_scenery_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_WALLS, entryIndex);
    if (obj != nullptr)
    {
        result = (rct_scenery_entry*)obj->GetLegacyData();
    }
    return result;
}

rct_scenery_entry* get_banner_entry(int32_t entryIndex)
{
    rct_scenery_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_BANNERS, entryIndex);
    if (obj != nullptr)
    {
        result = (rct_scenery_entry*)obj->GetLegacyData();
    }
    return result;
}

rct_scenery_entry* get_footpath_item_entry(int32_t entryIndex)
{
    rct_scenery_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_PATH_BITS, entryIndex);
    if (obj != nullptr)
    {
        result = (rct_scenery_entry*)obj->GetLegacyData();
    }
    return result;
}

rct_scenery_group_entry* get_scenery_group_entry(int32_t entryIndex)
{
    rct_scenery_group_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_SCENERY_GROUP, entryIndex);
    if (obj != nullptr)
    {
        result = (rct_scenery_group_entry*)obj->GetLegacyData();
    }
    return result;
}

int32_t get_scenery_id_from_entry_index(uint8_t objectType, int32_t entryIndex)
{
    switch (objectType)
    {
        case OBJECT_TYPE_SMALL_SCENERY:
            return entryIndex + SCENERY_SMALL_SCENERY_ID_MIN;
        case OBJECT_TYPE_PATH_BITS:
            return entryIndex + SCENERY_PATH_SCENERY_ID_MIN;
        case OBJECT_TYPE_WALLS:
            return entryIndex + SCENERY_WALLS_ID_MIN;
        case OBJECT_TYPE_LARGE_SCENERY:
            return entryIndex + SCENERY_LARGE_SCENERY_ID_MIN;
        case OBJECT_TYPE_BANNERS:
            return entryIndex + SCENERY_BANNERS_ID_MIN;
        default:
            return -1;
    }
}

int32_t wall_entry_get_door_sound(const rct_scenery_entry* wallEntry)
{
    return (wallEntry->wall.flags2 & WALL_SCENERY_2_DOOR_SOUND_MASK) >> WALL_SCENERY_2_DOOR_SOUND_SHIFT;
}
