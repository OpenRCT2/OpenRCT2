/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
    int16_t _x;
    int16_t _y;
    uint8_t _baseHeight;
    uint8_t _quadrant;
    uint8_t _sceneryType;

public:
    SmallSceneryRemoveAction() = default;

    SmallSceneryRemoveAction(int16_t x, int16_t y, uint8_t baseHeight, uint8_t quadrant, uint8_t sceneryType)
        : _x(x)
        , _y(y)
        , _baseHeight(baseHeight)
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

        stream << DS_TAG(_x) << DS_TAG(_y) << DS_TAG(_baseHeight) << DS_TAG(_quadrant) << DS_TAG(_sceneryType);
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();

        if (!map_is_location_valid({ _x, _y }))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        rct_scenery_entry* entry = get_small_scenery_entry(_sceneryType);
        if (entry == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res->Cost = entry->small_scenery.removal_price * 10;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        res->Position.x = _x;
        res->Position.y = _y;
        res->Position.z = _baseHeight * 8;

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !gCheatsSandboxMode)
        {
            // Check if allowed to remove item
            if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
            {
                if (entry->small_scenery.height > 64)
                {
                    res->Error = GA_ERROR::NO_CLEARANCE;
                    res->ErrorTitle = STR_CANT_REMOVE_THIS;
                    res->ErrorMessage = STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY;
                    return res;
                }
            }

            // Check if the land is owned
            if (!map_is_location_owned(_x, _y, _baseHeight * 8))
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
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        res->Position.x = _x;
        res->Position.y = _y;
        res->Position.z = _baseHeight * 8;

        TileElement* tileElement = FindSceneryElement();
        if (tileElement == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res->Position.z = tile_element_height(res->Position.x, res->Position.y);

        map_invalidate_tile_full(_x, _y);
        tile_element_remove(tileElement);

        return res;
    }

private:
    TileElement* FindSceneryElement() const
    {
        TileElement* tileElement = map_get_first_element_at(_x / 32, _y / 32);
        if (!tileElement)
            return nullptr;

        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;
            if ((tileElement->AsSmallScenery()->GetSceneryQuadrant()) != _quadrant)
                continue;
            if (tileElement->base_height != _baseHeight)
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
