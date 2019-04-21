/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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

DEFINE_GAME_ACTION(LargeSceneryRemoveAction, GAME_COMMAND_REMOVE_LARGE_SCENERY, GameActionResult)
{
private:
    int16_t _x;
    int16_t _y;
    uint8_t _baseHeight;
    uint8_t _direction;
    uint16_t _tileIndex;

public:
    LargeSceneryRemoveAction() = default;

    LargeSceneryRemoveAction(int16_t x, int16_t y, uint8_t baseHeight, uint8_t direction, uint16_t tileIndex)
        : _x(x)
        , _y(y)
        , _baseHeight(baseHeight)
        , _direction(direction)
        , _tileIndex(tileIndex)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_x) << DS_TAG(_y) << DS_TAG(_baseHeight) << DS_TAG(_direction) << DS_TAG(_tileIndex);
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();

        const uint32_t flags = GetFlags();

        int32_t z = tile_element_height(_x, _y);
        res->Position.x = _x + 16;
        res->Position.y = _y + 16;
        res->Position.z = z;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        res->Cost = 0;

        TileElement* tileElement = FindLargeSceneryElement();
        if (tileElement == nullptr)
        {
            log_warning("Invalid game command for scenery removal, x = %d, y = %d", _x, _y);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        rct_scenery_entry* scenery_entry = tileElement->AsLargeScenery()->GetEntry();

        LocationXYZ16 firstTile = {
            scenery_entry->large_scenery.tiles[_tileIndex].x_offset, scenery_entry->large_scenery.tiles[_tileIndex].y_offset,
            static_cast<int16_t>((_baseHeight * 8) - scenery_entry->large_scenery.tiles[_tileIndex].z_offset)
        };

        rotate_map_coordinates(&firstTile.x, &firstTile.y, _direction);

        firstTile.x = _x - firstTile.x;
        firstTile.y = _y - firstTile.y;

        bool calculate_cost = true;
        for (int32_t i = 0; scenery_entry->large_scenery.tiles[i].x_offset != -1; i++)
        {
            LocationXYZ16 currentTile = { scenery_entry->large_scenery.tiles[i].x_offset,
                                          scenery_entry->large_scenery.tiles[i].y_offset,
                                          scenery_entry->large_scenery.tiles[i].z_offset };

            rotate_map_coordinates(&currentTile.x, &currentTile.y, _direction);

            currentTile.x += firstTile.x;
            currentTile.y += firstTile.y;
            currentTile.z += firstTile.z;

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
            {
                if (!map_is_location_owned(currentTile.x, currentTile.y, currentTile.z))
                {
                    return MakeResult(GA_ERROR::NO_CLEARANCE, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
                }
            }

            // Prevent duplicate costs when using the clear scenery tool that overlaps multiple large
            // scenery tile elements.
            if (flags & GAME_COMMAND_FLAG_PATH_SCENERY)
            {
                if (tileElement->AsLargeScenery()->IsAccounted())
                    calculate_cost = false;

                // Sets the flag to prevent this being counted in additional calls
                tileElement->AsLargeScenery()->SetIsAccounted(true);
            }
        }

        if (calculate_cost)
            res->Cost = scenery_entry->large_scenery.removal_price * 10;

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();

        const uint32_t flags = GetFlags();

        int32_t z = tile_element_height(_x, _y);
        res->Position.x = _x + 16;
        res->Position.y = _y + 16;
        res->Position.z = z;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        res->Cost = 0;

        TileElement* tileElement = FindLargeSceneryElement();
        if (tileElement == nullptr)
        {
            log_warning("Invalid game command for scenery removal, x = %d, y = %d", _x, _y);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        tile_element_remove_banner_entry(tileElement);

        rct_scenery_entry* scenery_entry = tileElement->AsLargeScenery()->GetEntry();

        LocationXYZ16 firstTile = {
            scenery_entry->large_scenery.tiles[_tileIndex].x_offset, scenery_entry->large_scenery.tiles[_tileIndex].y_offset,
            static_cast<int16_t>((_baseHeight * 8) - scenery_entry->large_scenery.tiles[_tileIndex].z_offset)
        };

        rotate_map_coordinates(&firstTile.x, &firstTile.y, _direction);

        firstTile.x = _x - firstTile.x;
        firstTile.y = _y - firstTile.y;

        for (int32_t i = 0; scenery_entry->large_scenery.tiles[i].x_offset != -1; i++)
        {
            LocationXYZ16 currentTile = { scenery_entry->large_scenery.tiles[i].x_offset,
                                          scenery_entry->large_scenery.tiles[i].y_offset,
                                          scenery_entry->large_scenery.tiles[i].z_offset };

            rotate_map_coordinates(&currentTile.x, &currentTile.y, _direction);

            currentTile.x += firstTile.x;
            currentTile.y += firstTile.y;
            currentTile.z += firstTile.z;

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
            {
                if (!map_is_location_owned(currentTile.x, currentTile.y, currentTile.z))
                {
                    return MakeResult(GA_ERROR::NO_CLEARANCE, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
                }
            }

            TileElement* sceneryElement = map_get_first_element_at(currentTile.x / 32, currentTile.y / 32);
            bool element_found = false;
            do
            {
                if (sceneryElement->GetType() != TILE_ELEMENT_TYPE_LARGE_SCENERY)
                    continue;

                if (sceneryElement->GetDirection() != _direction)
                    continue;

                if (sceneryElement->AsLargeScenery()->GetSequenceIndex() != i)
                    continue;

                if (sceneryElement->base_height != currentTile.z / 8)
                    continue;

                // If we are removing ghost elements
                if ((flags & GAME_COMMAND_FLAG_GHOST) && sceneryElement->IsGhost() == false)
                    continue;

                map_invalidate_tile_full(currentTile.x, currentTile.y);
                tile_element_remove(sceneryElement);

                element_found = true;
                break;
            } while (!(sceneryElement++)->IsLastForTile());

            if (element_found == false)
            {
                log_error("Tile not found when trying to remove element!");
            }
        }

        res->Cost = scenery_entry->large_scenery.removal_price * 10;

        return res;
    }

private:
    TileElement* FindLargeSceneryElement() const
    {
        TileElement* tileElement = map_get_first_element_at(_x / 32, _y / 32);
        if (tileElement == nullptr)
            return nullptr;

        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_LARGE_SCENERY)
                continue;

            if (tileElement->base_height != _baseHeight)
                continue;

            if (tileElement->AsLargeScenery()->GetSequenceIndex() != _tileIndex)
                continue;

            if (tileElement->GetDirection() != _direction)
                continue;

            // If we are removing ghost elements
            if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && tileElement->IsGhost() == false)
                continue;

            return tileElement;

        } while (!(tileElement++)->IsLastForTile());

        return nullptr;
    }
};
