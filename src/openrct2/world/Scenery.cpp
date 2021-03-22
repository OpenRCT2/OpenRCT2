/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../actions/BannerRemoveAction.h"
#include "../actions/FootpathAdditionRemoveAction.h"
#include "../actions/LargeSceneryRemoveAction.h"
#include "../actions/SmallSceneryRemoveAction.h"
#include "../actions/WallRemoveAction.h"
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

uint8_t gSceneryQuadrant;

money32 gSceneryPlaceCost;
ScenerySelection gSceneryPlaceObject;
int16_t gSceneryPlaceZ;
uint8_t gSceneryPlaceRotation;

uint8_t gSceneryGhostType;
CoordsXYZ gSceneryGhostPosition;
uint8_t gSceneryGhostWallRotation;

int16_t gSceneryShiftPressed;
int16_t gSceneryShiftPressX;
int16_t gSceneryShiftPressY;
int16_t gSceneryShiftPressZOffset;

int16_t gSceneryCtrlPressed;
int16_t gSceneryCtrlPressZ;

money32 gClearSceneryCost;

// rct2: 0x009A3E74
const CoordsXY SceneryQuadrantOffsets[] = { { 7, 7 }, { 7, 23 }, { 23, 23 }, { 23, 7 } };

void scenery_update_tile(const CoordsXY& sceneryPos)
{
    TileElement* tileElement;

    tileElement = map_get_first_element_at(sceneryPos);
    if (tileElement == nullptr)
        return;
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
            tileElement->AsSmallScenery()->UpdateAge(sceneryPos);
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
                        JumpingFountain::StartAnimation(JumpingFountainType::Water, sceneryPos, tileElement);
                    }
                    else if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW)
                    {
                        JumpingFountain::StartAnimation(JumpingFountainType::Snow, sceneryPos, tileElement);
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
void SmallSceneryElement::UpdateAge(const CoordsXY& sceneryPos)
{
    auto* sceneryEntry = GetEntry();
    if (sceneryEntry == nullptr)
    {
        return;
    }

    if (gCheatsDisablePlantAging && (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_CAN_BE_WATERED)))
    {
        return;
    }

    if (!scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_CAN_BE_WATERED) || WeatherIsDry(gClimateCurrent.Weather)
        || GetAge() < 5)
    {
        IncreaseAge(sceneryPos);
        return;
    }

    // Check map elements above, presumably to see if map element is blocked from weather
    TileElement* tileElementAbove = reinterpret_cast<TileElement*>(this);
    // Change from original: RCT2 only checked for the first three quadrants, which was very likely to be a bug.
    while (!(tileElementAbove->GetOccupiedQuadrants()))
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
                map_invalidate_tile_zoom1({ sceneryPos, tileElementAbove->GetBaseZ(), tileElementAbove->GetClearanceZ() });
                IncreaseAge(sceneryPos);
                return;
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                sceneryEntry = tileElementAbove->AsSmallScenery()->GetEntry();
                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
                {
                    IncreaseAge(sceneryPos);
                    return;
                }
                break;
        }
    }

    // Reset age / water plant
    SetAge(0);
    map_invalidate_tile_zoom1({ sceneryPos, GetBaseZ(), GetClearanceZ() });
}

/**
 *
 *  rct2: 0x006E2712
 */
void scenery_remove_ghost_tool_placement()
{
    if (gSceneryGhostType & SCENERY_GHOST_FLAG_0)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_0;

        auto removeSceneryAction = SmallSceneryRemoveAction(
            gSceneryGhostPosition, gSceneryQuadrant, gSceneryPlaceObject.EntryIndex);
        removeSceneryAction.SetFlags(
            GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
        removeSceneryAction.Execute();
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_1)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_1;
        TileElement* tileElement = map_get_first_element_at(gSceneryGhostPosition);

        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
                continue;

            if (tileElement->GetBaseZ() != gSceneryGhostPosition.z)
                continue;

            auto footpathAdditionRemoveAction = FootpathAdditionRemoveAction(gSceneryGhostPosition);
            footpathAdditionRemoveAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST);
            GameActions::Execute(&footpathAdditionRemoveAction);
            break;
        } while (!(tileElement++)->IsLastForTile());
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_2)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_2;

        CoordsXYZD wallLocation = { gSceneryGhostPosition, gSceneryGhostWallRotation };
        auto wallRemoveAction = WallRemoveAction(wallLocation);
        wallRemoveAction.SetFlags(GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_PATH_SCENERY);
        wallRemoveAction.Execute();
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_3)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_3;

        auto removeSceneryAction = LargeSceneryRemoveAction({ gSceneryGhostPosition, gSceneryPlaceRotation }, 0);
        removeSceneryAction.SetFlags(
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
            | GAME_COMMAND_FLAG_NO_SPEND);
        removeSceneryAction.Execute();
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_4)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_4;

        auto removeSceneryAction = BannerRemoveAction({ gSceneryGhostPosition, gSceneryPlaceRotation });
        removeSceneryAction.SetFlags(
            GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
        GameActions::Execute(&removeSceneryAction);
    }
}

rct_scenery_entry* get_wall_entry(ObjectEntryIndex entryIndex)
{
    rct_scenery_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::Walls, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<rct_scenery_entry*>(obj->GetLegacyData());
    }
    return result;
}

rct_scenery_entry* get_banner_entry(ObjectEntryIndex entryIndex)
{
    rct_scenery_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::Banners, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<rct_scenery_entry*>(obj->GetLegacyData());
    }
    return result;
}

rct_scenery_entry* get_footpath_item_entry(ObjectEntryIndex entryIndex)
{
    rct_scenery_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::PathBits, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<rct_scenery_entry*>(obj->GetLegacyData());
    }
    return result;
}

rct_scenery_group_entry* get_scenery_group_entry(ObjectEntryIndex entryIndex)
{
    rct_scenery_group_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::SceneryGroup, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<rct_scenery_group_entry*>(obj->GetLegacyData());
    }
    return result;
}

int32_t wall_entry_get_door_sound(const rct_scenery_entry* wallEntry)
{
    return (wallEntry->wall.flags2 & WALL_SCENERY_2_DOOR_SOUND_MASK) >> WALL_SCENERY_2_DOOR_SOUND_SHIFT;
}
