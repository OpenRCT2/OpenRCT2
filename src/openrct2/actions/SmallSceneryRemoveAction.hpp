/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../world/Park.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(SmallSceneryRemoveAction, GAME_COMMAND_REMOVE_SCENERY, GameActionResult)
{
private:
    CoordsXYZ _loc;
    uint8_t _quadrant;
    ObjectEntryIndex _sceneryType;

public:
    SmallSceneryRemoveAction() = default;

    SmallSceneryRemoveAction(const CoordsXYZ& location, uint8_t quadrant, ObjectEntryIndex sceneryType)
        : _loc(location)
        , _quadrant(quadrant)
        , _sceneryType(sceneryType)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_quadrant) << DS_TAG(_sceneryType);
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();

        if (!LocationValid(_loc))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        rct_scenery_entry* entry = get_small_scenery_entry(_sceneryType);
        if (entry == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res->Cost = entry->small_scenery.removal_price * 10;
        res->Expenditure = ExpenditureType::Landscaping;
        res->Position = _loc;

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !gCheatsSandboxMode)
        {
            // Check if allowed to remove item
            if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
            {
                if (scenery_small_entry_has_flag(entry, SMALL_SCENERY_FLAG_IS_TREE))
                {
                    res->Error = GA_ERROR::NO_CLEARANCE;
                    res->ErrorTitle = STR_CANT_REMOVE_THIS;
                    res->ErrorMessage = STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY;
                    return res;
                }
            }

            // Check if the land is owned
            if (!map_is_location_owned(_loc))
            {
                res->Error = GA_ERROR::NO_CLEARANCE;
                res->ErrorTitle = STR_CANT_REMOVE_THIS;
                res->ErrorMessage = STR_LAND_NOT_OWNED_BY_PARK;
                return res;
            }
        }

        TileElement* tileElement = FindSceneryElement();
        if (tileElement == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();

        rct_scenery_entry* entry = get_small_scenery_entry(_sceneryType);
        if (entry == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res->Cost = entry->small_scenery.removal_price * 10;
        res->Expenditure = ExpenditureType::Landscaping;
        res->Position = _loc;

        TileElement* tileElement = FindSceneryElement();
        if (tileElement == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res->Position.z = tile_element_height(res->Position);

        map_invalidate_tile_full(_loc);
        tile_element_remove(tileElement);

        return res;
    }

private:
    TileElement* FindSceneryElement() const
    {
        TileElement* tileElement = map_get_first_element_at(_loc);
        if (!tileElement)
            return nullptr;

        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;
            if ((tileElement->AsSmallScenery()->GetSceneryQuadrant()) != _quadrant)
                continue;
            if (tileElement->GetBaseZ() != _loc.z)
                continue;
            if (tileElement->AsSmallScenery()->GetEntryIndex() != _sceneryType)
                continue;
            if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && tileElement->IsGhost() == false)
                continue;

            return tileElement;

        } while (!(tileElement++)->IsLastForTile());

        return nullptr;
    }
};
